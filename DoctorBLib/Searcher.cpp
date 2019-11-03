#include "stdafx.h"
#include "Searcher.h"
#include <iostream>
#include <chrono>
#include "MoveGenerator.h"
#include "Evaluator.h"

using namespace std;

Searcher::Searcher(Position base_position) : base_position_(base_position) {
	node_count = 0;
}

void Searcher::GoDepth(int depth) {
	Move best_move = MinMaxDepth(depth);
	cout << "bestmove " << best_move.ToString() << endl;
}

void Searcher::GoTime(int wtime, int btime, int winc, int binc) {
	Move best_move = MinMaxTime(wtime, btime, winc, binc);
	cout << "bestmove " << best_move.ToString() << endl;
}

Searcher::~Searcher() {
}


Move Searcher::MinMaxDepth(uint32_t max_depth) {
	node_count = 0Ui64;
	auto start_time = chrono::system_clock::now();
	Move best_move;

	for (uint32_t iteration_depth = 1; iteration_depth <= max_depth; iteration_depth++) {
		vector<Move> pv(max_depth + 1);
		Score score;
		MinMaxRecurse(iteration_depth, base_position_, score, pv);

		auto end_time = chrono::system_clock::now();
		chrono::duration<double> elapsed_seconds = end_time - start_time;
		int nps = elapsed_seconds.count() > 0 ? (int)(node_count / elapsed_seconds.count()) : 0;

		cout << "info depth " << iteration_depth << " score " << score.ToString(base_position_.GetActiveColor(), iteration_depth) << " nodes " << node_count << " nps " << nps;
		if (!score.IsMate()) {
			cout << " pv";
			for (int i = iteration_depth; i > 0; i--) {
				cout << " " << pv[i].ToString();
			}
		}
		cout << endl;

		if (score.IsMate())
			break;

		best_move = pv[iteration_depth];
	}

	return best_move;
}

Move Searcher::MinMaxTime(int wtime, int btime, int winc, int binc) {
	node_count = 0Ui64;

	int max_duration = (base_position_.GetActiveColor() == Piece::COLOR_WHITE) ? (wtime / 40 + winc) : (btime / 40 + binc);
	auto start_time = chrono::system_clock::now();
	Move best_move;
	uint32_t iteration_depth = 1;
	int duration = 0;

	do {
		vector<Move> pv(iteration_depth + 1);
		Score score;
		MinMaxRecurse(iteration_depth, base_position_, score, pv);

		auto end_time = chrono::system_clock::now();
		chrono::duration<double> elapsed_seconds = end_time - start_time;
		duration = (int)(elapsed_seconds.count() * 1000);

		int nps = elapsed_seconds.count() > 0 ? (int)(node_count / elapsed_seconds.count()) : 0;

		cout << "info depth " << iteration_depth << " score " << score.ToString(base_position_.GetActiveColor(), iteration_depth) << " nodes " << node_count << " nps " << nps;
		if (!score.IsMate()) {
			cout << " pv";
			for (int i = iteration_depth; i > 0; i--) {
				cout << " " << pv[i].ToString();
			}
		}
		cout << endl;
		
		if (score.IsMate())
			break;

		best_move = pv[iteration_depth];

		iteration_depth++;
	} while (duration < (max_duration / 20));


	return best_move;
}

void Searcher::MinMaxRecurse(const uint32_t remaining_depth, const Position& position, Score& score, std::vector<Move>& pv) {
	static const int64_t START_SCORE[2] = { Score::WHITE_START_SCORE, Score::BLACK_START_SCORE };

	if (remaining_depth == 0) {
		Evaluator eval(position);
		eval.Evaluate(score);
		return;
	}

	vector<Move> moves;
	vector<Move> child_variant(remaining_depth + 1);
	MoveGenerator move_generator(position);
	move_generator.GenerateMoves(moves);
	Score best_score;
	Move best_move;
	
	score.SetValue(START_SCORE[position.GetActiveColor()]);

	for (Move move : moves) {
		Position new_position(position);
		new_position.ApplyMove(move);
		node_count++;
		Score new_score;
		MinMaxRecurse(remaining_depth - 1, new_position, new_score, child_variant);
		if (Evaluator::CompareScore(position.GetActiveColor(), new_score, score) > 0) {
			score.SetValue(new_score.GetValue());
			pv[remaining_depth] = move;
			//variant array has the moves in reverse order; pv[1] is the last move; pv[remaining_depth] is the first move
			//element at [0] is not used
			//copy moves after the current move
			if (remaining_depth > 1)
				copy(child_variant.begin() + 1, child_variant.begin() + remaining_depth, pv.begin() + 1);
		}
	}
}
