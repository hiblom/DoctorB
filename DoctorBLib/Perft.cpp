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
	return Count2(position_, depth_);
}

//Count function using recursion
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

//Count function using a loop instead of recursion
uint64_t Perft::Count2(const Position& position, const int remaining_depth) const {
	vector<uint64_t> results(128);
	uint64_t result = 0;
	
	vector<Position> positions(depth_ + 1);
	vector<vector<Move>> moves(depth_ + 1);
	vector<int> move_indices(depth_ + 1, -1);

	int current_depth = remaining_depth;
	positions[current_depth] = position;
	
	while (current_depth <= depth_) {
		if (move_indices[current_depth] == -1) {
			MoveGenerator move_gen(positions[current_depth]);

			moves[current_depth].clear();
			move_gen.GenerateMoves(moves[current_depth]);
			if (current_depth == 1) {
				if (depth_ == 1) {
					for (int i = 0; i < moves[current_depth].size(); i++) {
						results[i] = 1;
					}
					break;
				}

				results[move_indices[depth_]] += moves[current_depth].size();
				current_depth++;
				continue;

			}
		}

		move_indices[current_depth]++;

		if (move_indices[current_depth] >= moves[current_depth].size()) {
			move_indices[current_depth] = -1;
			current_depth++;
			continue;
		} 
		
		positions[current_depth - 1] = Position(positions[current_depth]);
		positions[current_depth - 1].ApplyMove(moves[current_depth][move_indices[current_depth]]);
		current_depth--;
	}

	for (int i = 0; i < moves[depth_].size(); i++) {
		if (results[i]) {
			cout << moves[depth_][i].ToString() << ": " << results[i] << endl;
			result += results[i];
		}
	}

	return result;
}
