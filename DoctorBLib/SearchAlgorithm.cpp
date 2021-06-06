#include "stdafx.h"
#include "SearchAlgorithm.h"
#include <iostream>
#include <chrono>
#include <algorithm>

using namespace std;

Move SearchAlgorithm::goDepth(uint64_t max_depth) {
	node_count_ = 0Ui64;
	auto start_time = chrono::system_clock::now();
	Move best_move;

	string pv_string;
	for (uint32_t iteration_depth = 1; iteration_depth <= max_depth; iteration_depth++) {
		vector<Move> pv(iteration_depth);
		Score score;
		loop(iteration_depth, score, pv);

		auto end_time = chrono::system_clock::now();
		chrono::duration<double> elapsed_seconds = end_time - start_time;
		int nps = elapsed_seconds.count() > 0 ? static_cast<int>(node_count_ / elapsed_seconds.count()) : 0;

		cout << "info depth " << iteration_depth << " score " << score.toString(base_position_.getActiveColor(), iteration_depth) << " nodes " << node_count_ << " nps " << nps;

		pv_string = " pv";
		for (Move move : pv) {
			pv_string += " " + move.toString();
		}

		cout << pv_string << endl;

		if (pv[0].isValid()) {
			best_move = pv[0];
		}

		afterIteration();

		if (score.isMate())
			break;

	}

	afterSearch();

	return best_move;
}

Move SearchAlgorithm::goTime(uint64_t max_duration) {
	node_count_ = 0Ui64;
	auto start_time = chrono::system_clock::now();
	Move best_move;
	uint32_t iteration_depth = 1;
	int duration = 0;
	string pv_string;

	do {
		vector<Move> pv(iteration_depth);
		Score score;
		loop(iteration_depth, score, pv);

		auto end_time = chrono::system_clock::now();
		chrono::duration<double> elapsed_seconds = end_time - start_time;
		duration = static_cast<int>(elapsed_seconds.count() * 1000);
		int nps = elapsed_seconds.count() > 0 ? static_cast<int>(node_count_ / elapsed_seconds.count()) : 0;

		cout << "info depth " << iteration_depth << " score " << score.toString(base_position_.getActiveColor(), iteration_depth) << " nodes " << node_count_ << " nps " << nps;

		pv_string = " pv";
		for (Move move : pv) {
			pv_string += " " + move.toString();
		}

		cout << pv_string << endl;
		
		if (pv[0].isValid()) {
			best_move = pv[0];
		}

		afterIteration();

		if (score.isMate())
			break;

		iteration_depth++;
	} while ((duration * getSearchTimeMultiplier()) < max_duration); //TODO implement game modes like CCRL 40/4

	afterSearch();

	return best_move;
}
