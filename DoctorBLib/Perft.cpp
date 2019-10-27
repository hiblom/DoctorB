#include "stdafx.h"
#include "Perft.h"
#include <vector>
#include <iostream>
#include "Move.h"
#include "MoveGenerator.h"

using namespace std;

Perft::Perft(const Position& position) : position_(position) {
}

Perft::~Perft() {
}

void Perft::SetDepth(int value) {
	depth_ = value;
}

uint64_t Perft::Go() {
	return Count(position_, depth_);
}

uint64_t Perft::Count(const Position& position, const int remaining_depth) const {
	MoveGenerator move_gen(position);
	if (move_gen.IsCheck(position.GetActiveColor() ^ 1Ui8))
		return 0Ui64;

	if (remaining_depth == 0)
		return 1Ui64;

	uint64_t result = 0;
	vector<Move> raw_moves;
	Position new_position;
	move_gen.GenerateMoves(raw_moves);
	for (Move raw_move : raw_moves) {
		new_position = Position(position);
		new_position.ApplyMove(raw_move);
		uint64_t count = Count(new_position, remaining_depth - 1);
		if (remaining_depth == depth_ && count > 0)
			cout << raw_move.ToString() << ": " << count << endl;
		result += count;
	}
	return result;
}
