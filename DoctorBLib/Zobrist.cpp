#include "stdafx.h"
#include "Zobrist.h"
#include <random>

using namespace std;

Zobrist::Zobrist() {
	Initialize();
}

Zobrist::~Zobrist() {
}

Zobrist& Zobrist::GetInstance() {
	static Zobrist instance;
	return instance;
}

void Zobrist::Initialize() {
	mt19937_64 gen(SEED);

	for (int p = 0; p < 12; p++) {
		for (int s = 0; s < 64; s++) {
			PIECE_SQUARE_KEY[p][s] = gen();
		}
	}

	BLACK_TURN_KEY = gen();

	for (int i = 0; i < 4; i++) {
		CASTLING_KEY[i] = gen();
	}

	for (int f = 0; f < 8; f++) {
		EP_FILE_KEY[f] = gen();
	}
}
