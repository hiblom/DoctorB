#include "stdafx.h"
#include "SearchAlgorithm.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include "Globals.h"
#include "Console.h"
#include "TranspositionTable.h"

using namespace std;

const array<uint64_t, 4> SearchAlgorithm::BOUND_INCREMENTS { { 25, 100, 300, Score::BLACK_START_SCORE } };

Move SearchAlgorithm::goDepth(uint64_t max_depth) {
	node_count_ = 0Ui64;
	auto start_time = chrono::system_clock::now();
	Move best_move;
	array<int, 2> bounds { { Score::BLACK_START_SCORE , Score::WHITE_START_SCORE } }; //black ubound; white lbound
	int bound_index = 0;

	string pv_string;
	for (uint32_t iteration_depth = 1; iteration_depth <= max_depth;) {
		vector<Move> pv(iteration_depth);
		Score score;

		//cout << "searching with bounds " << bounds[1] << "," << bounds[0] << endl;
		loop(iteration_depth, score, pv, bounds);
		
		if (Globals::stop)
			break;
		
		if (score.getValue() > bounds[Piece::COLOR_BLACK]) {
			bounds[Piece::COLOR_BLACK] += BOUND_INCREMENTS[++bound_index];
			continue;
		}
		else if (score.getValue() < bounds[Piece::COLOR_WHITE] ) {
			bounds[Piece::COLOR_WHITE] -= BOUND_INCREMENTS[++bound_index];
			continue;
		}
		else {
			bound_index = 0;
			bounds[Piece::COLOR_BLACK] = score.getValue() + BOUND_INCREMENTS[0];
			bounds[Piece::COLOR_WHITE] = score.getValue() - BOUND_INCREMENTS[0];
		}

		auto end_time = chrono::system_clock::now();
		chrono::duration<double> elapsed_seconds = end_time - start_time;
		int nps = elapsed_seconds.count() > 0 ? static_cast<int>(node_count_ / elapsed_seconds.count()) : 0;

		Console::getInstance() << "info depth " << iteration_depth << " score " << score.toString(base_position_.getActiveColor(), iteration_depth) << " nodes " << node_count_ << " nps " << nps;

		pv_string = " pv";
		for (Move move : pv) {
			pv_string += " " + move.toString();
		}

		Console::getInstance() << pv_string << endl;

		if (pv[0].isValid()) {
			best_move = pv[0];
		}

		afterIteration();

		if (score.isMate())
			break;

		iteration_depth++;
	}

	afterSearch();

	return best_move;
}

Move SearchAlgorithm::goTime(uint64_t max_duration) {
	node_count_ = 0Ui64;
	auto start_time = chrono::system_clock::now();
	Move best_move;
	array<int, 2> bounds{ { Score::BLACK_START_SCORE , Score::WHITE_START_SCORE } }; //black ubound; white lbound
	uint32_t iteration_depth = 1;
	int duration = 0;
	string pv_string;
	int bound_index = 0;

	while ((duration * getSearchTimeMultiplier()) < max_duration) {
		vector<Move> pv(iteration_depth);
		Score score;

		//cout << "searching with bounds " << bounds[1] << "," << bounds[0] << endl;
		loop(iteration_depth, score, pv, bounds);

		if (Globals::stop)
			break;

		auto end_time = chrono::system_clock::now();
		chrono::duration<double> elapsed_seconds = end_time - start_time;
		duration = static_cast<int>(elapsed_seconds.count() * 1000);

		if (score.getValue() > bounds[Piece::COLOR_BLACK]) {
			bounds[Piece::COLOR_BLACK] += BOUND_INCREMENTS[++bound_index];
			continue;
		}
		else if (score.getValue() < bounds[Piece::COLOR_WHITE]) {
			bounds[Piece::COLOR_WHITE] -= BOUND_INCREMENTS[++bound_index];
			continue;
		}
		else {
			bound_index = 0;
			bounds[Piece::COLOR_BLACK] = score.getValue() + BOUND_INCREMENTS[0];
			bounds[Piece::COLOR_WHITE] = score.getValue() - BOUND_INCREMENTS[0];
		}

		int nps = elapsed_seconds.count() > 0 ? static_cast<int>(node_count_ / elapsed_seconds.count()) : 0;

		Console::getInstance() << "info depth " << iteration_depth << " score " << score.toString(base_position_.getActiveColor(), iteration_depth) << " nodes " << node_count_ << " nps " << nps;

		pv_string = " pv";
		for (Move move : pv) {
			pv_string += " " + move.toString();
		}

		Console::getInstance() << pv_string << endl;
		
		if (pv[0].isValid()) {
			best_move = pv[0];
		}

		afterIteration();

		if (score.isMate())
			break;

		iteration_depth++;
	};

	afterSearch();

	return best_move;
}
