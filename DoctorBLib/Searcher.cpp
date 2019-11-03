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
	Move best_move = MinMax(depth);
	cout << "best move " << best_move.ToString() << endl;
}

Searcher::~Searcher() {
}


Move Searcher::MinMax(uint32_t max_depth) {
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

		cout << "info depth " << iteration_depth << " score cp " << score.GetValue() << " nodes " << node_count << " nps " << nps << " pv";
		for (int i = iteration_depth; i > 0; i--) {
			cout << " " << pv[i].ToString();
		}
		cout << endl;

		best_move = pv[iteration_depth];
	}

	return best_move;
}

void Searcher::MinMaxRecurse(const uint32_t remaining_depth, const Position& position, Score& score, std::vector<Move>& pv) {
	static const int64_t START_SCORE[2] = { -32767, 32767 };

	if (remaining_depth == 0) {
		Evaluator eval(position);
		eval.Evaluate(score);
		return;
	}

	vector<Move> moves;
	MoveGenerator move_generator(position);
	move_generator.GenerateMoves(moves);
	Score best_score;
	Move best_move;
	
	best_score.SetValue(START_SCORE[position.GetActiveColor()]);

	for (Move move : moves) {
		Position new_position(position);
		new_position.ApplyMove(move);
		node_count++;
		Score new_score;
		MinMaxRecurse(remaining_depth - 1, new_position, new_score, pv);
		if (Evaluator::CompareScore(position.GetActiveColor(), new_score, best_score) > 0) {
			best_score = new_score;
			best_move = move;
		}
	}

	score.SetValue(best_score.GetValue());
	pv[remaining_depth] = best_move;
}
