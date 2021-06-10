#include "stdafx.h"
#include "Perft.h"
#include <vector>
#include <iostream>
#include "Move.h"
#include "MoveGenerator.h"

using namespace std;

Perft::Perft(const Position& position) : position_(position), depth_(0) {
}

Perft::~Perft() {
}

void Perft::setDepth(int value) {
	depth_ = value;
}

uint64_t Perft::go() const {
	uint64_t result = 0;
	array<uint64_t, 128> results {};

	vector<State> states(depth_);
	states[0].position = position_;
	int max_depth = depth_ - 1;

	State& root_state = states[0];
	int current_depth = 0;
	while (current_depth >= 0) {
		State& current_state = states[current_depth];

		if (current_state.move_index == -1) {
			current_state.generateMoves();
		}

		if (current_depth >= max_depth) {
			if (current_depth > 0) {
				results[root_state.move_index] += current_state.moves.size();
			}
			else {
				for (int i = 0; i < current_state.moves.size(); i++) {
					results[i] = 1;
				}
			}
			current_state.move_index = -1;
			current_depth--;
			continue;
		}
		
		current_state.move_index++;

		if (current_state.move_index >= current_state.moves.size()) {
			current_state.move_index = -1;
			current_depth--;
			continue;
		}

		current_depth++;
		State& child_state = states[current_depth];
		child_state.position = Position(current_state.position);
		child_state.position.applyMove(current_state.moves[current_state.move_index]);
	}

	for (int i = 0; i < states[0].moves.size(); i++) {
		if (results[i]) {
			cout << states[0].moves[i].toString() << ": " << results[i] << endl;
			result += results[i];
		}
	}

	return result;
}
