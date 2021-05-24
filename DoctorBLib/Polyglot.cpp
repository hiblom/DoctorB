#include "stdafx.h"
#include "Polyglot.h"
#include <vector>
#include "Options.h"

using namespace std;

Polyglot& Polyglot::get_instance() {
    if (instance_ == nullptr) {
        instance_ = new Polyglot();
    }
    return *instance_;
}

bool Polyglot::open() {
    if (file_ && Options::OwnBookPath == filePath_) {
        return true;
    }

    if (file_.is_open()) {
        file_.close();
    }

    if (Options::OwnBookPath == "") {
        return false;
    }

    filePath_ = Options::OwnBookPath;
    file_ = ifstream(filePath_, ios::in | ios::binary);
    if (!file_.is_open()) {
        return false;
    }

    return true;
}

Move Polyglot::get_move(uint64_t hash) {
    if (!file_ && file_.is_open()) {
        return Move();
    }

    bool found = false;
    
    //set file pointer to start
    file_.seekg(0, ios::beg);

    vector<PolyglotEntry> entries;
    
    int count = 0;
    int total_weight = 0;
    
    PolyglotEntry entry {};
    
    //collect all moves with right hash
    while (!file_.eof()) {
        read_entry(entry);

        count++;

        if (entry.hash < hash)
            continue;
        else if (entry.hash > hash)
            break;

        found = true;
        PolyglotEntry new_entry(entry);
        total_weight += entry.weight;
        entries.push_back(new_entry);
    }

    if (!found)
        return Move();

    //determine move (with rng)
    int r = get_random(total_weight);

    uint16_t entry_move = 0;
    int running_weight = 0;
    for (size_t i = 0; i < entries.size(); i++) {
        running_weight += entries[i].weight;
        if (running_weight > r) {
            entry_move = entries[i].move;
            break;
        }
    }

    return convert_move(entry_move);
}

Polyglot::~Polyglot() {
    if (file_) {
        file_.close();
    }
}

Polyglot::Polyglot() {
    open();
}

void Polyglot::read_entry(PolyglotEntry& entry) {
    file_.read((char*)&entry, sizeof(PolyglotEntry));
    
    //swap bytes because they are in the wrong order...little endian vs big endian
    entry.hash = _byteswap_uint64(entry.hash);
    entry.move = _byteswap_ushort(entry.move);
    entry.weight = _byteswap_ushort(entry.weight);
    //note: ignoring learn
}

Move Polyglot::convert_move(uint16_t polyglot_move) {

    const uint16_t TO_MASK = 0b0000000000111111;
    const uint8_t TO_OFFSET = 0;
    const uint16_t FROM_MASK = 0b0000111111000000;
    const uint8_t FROM_OFFSET = 6;
    const uint16_t PROMO_MASK = 0b0111000000000000;
    const uint8_t PROMO_OFFSET = 12;

    Square square_to = Square((uint8_t)((polyglot_move & TO_MASK) >> TO_OFFSET));
    Square square_from = Square((uint8_t)((polyglot_move & FROM_MASK) >> FROM_OFFSET));

    uint8_t polyglot_promo_piece = (uint8_t)((polyglot_move & PROMO_MASK) >> PROMO_OFFSET);
    if (polyglot_promo_piece == 0)
        return Move(square_from, square_to);

    uint8_t piece_value = 0;
    switch (polyglot_promo_piece)
    {
        case 1:  piece_value = Piece::TYPE_KNIGHT; break;
        case 2:  piece_value = Piece::TYPE_BISHOP; break;
        case 3:  piece_value = Piece::TYPE_ROOK; break;
        case 4:  piece_value = Piece::TYPE_QUEEN; break;
    }
    
    return Move(square_from, square_to, Piece(piece_value));
}

int Polyglot::get_random(int max) {
    return (rand() * RAND_MAX + rand()) % max;
}

Polyglot* Polyglot::instance_ = nullptr;