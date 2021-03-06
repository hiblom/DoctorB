#include "stdafx.h"
#include "TranspositionTable.h"
#include "Options.h"

using namespace std;

bool TranspositionTable::setBestMove(const uint64_t key, const Move move) {
	auto it = map->find(key);
	if (it == map->end()) {
		if (map->size() >= max_size)
			return false;

		Entry entry;
		entry.best_move = move;
		map->insert(pair<uint64_t, Entry>(key, entry));
		return true;
	}

	map->at(key).best_move = move;
	return true;
}

bool TranspositionTable::setScore(const uint64_t key, const Score score, uint16_t remaining_depth) {
	auto it = map->find(key);
	if (it == map->end()) {
		if (map->size() >= max_size)
			return false;

		Entry entry;
		entry.score = score;
		entry.remaining_depth = remaining_depth;
		map->insert(pair<uint64_t, Entry>(key, entry));
		return true;
	}

	map->at(key).score = score;
	map->at(key).remaining_depth = remaining_depth;
	return true;
}

bool TranspositionTable::findBestMove(const uint64_t key, Move& move) {
	auto it = map->find(key);
	if (it == map->end())
		return false;
	
	if (!it->second.best_move.has_value())
		return false;

	move = it->second.best_move.value();
	return true;
}

bool TranspositionTable::findScore(const uint64_t key, Score& score, uint16_t& remaining_depth) {
	auto it = map->find(key);
	if (it == map->end())
		return false;

	if (!it->second.score.has_value())
		return false;

	score = it->second.score.value();
	remaining_depth = it->second.remaining_depth;
	return true;
}

int TranspositionTable::getHashFull() {
	return static_cast<int>(map->size() * 1000000 / max_size);
}

void TranspositionTable::clear() {
	map->clear();
}

void TranspositionTable::reset() {
	int elem_size = sizeof(pair<uint64_t, Entry>);
	max_size = (Options::Hash * 1048576Ui32) / elem_size;
	map->clear();
	map->reserve(max_size);
}

TranspositionTable::TranspositionTable() {
	reset();
}

TranspositionTable::~TranspositionTable() {
}

TranspositionTable& TranspositionTable::getInstance() {
	static TranspositionTable instance;
	return instance;
}
