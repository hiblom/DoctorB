#include "stdafx.h"
#include "AlphaBeta.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include "MoveGenerator.h"
#include "Evaluator.h"

using namespace std;

AlphaBeta::AlphaBeta(const Position& base_position) : SearchAlgorithm(base_position) {
}

AlphaBeta::~AlphaBeta() {
}

Move AlphaBeta::GoDepth(uint64_t max_depth) {
	node_count_ = 0Ui64;
	auto start_time = chrono::system_clock::now();
	Move best_move;

	string pv_string;
	for (uint32_t iteration_depth = 1; iteration_depth <= max_depth; iteration_depth++) {
		vector<Move> pv(iteration_depth);
		Score score;
		Loop(iteration_depth, score, pv);

		auto end_time = chrono::system_clock::now();
		chrono::duration<double> elapsed_seconds = end_time - start_time;
		int nps = elapsed_seconds.count() > 0 ? (int)(node_count_ / elapsed_seconds.count()) : 0;

		cout << "info depth " << iteration_depth << " score " << score.ToString(base_position_.GetActiveColor(), iteration_depth) << " nodes " << node_count_ << " nps " << nps;
		if (score.IsMate()) {
			cout << pv_string << endl;
			break;
		}

		pv_string = " pv";
		for (Move move : pv) {
			pv_string += " " + move.ToString();
		}

		cout << pv_string << endl;
		best_move = pv[0];
	}

	return best_move;
}

Move AlphaBeta::GoTime(uint64_t max_duration) {
	node_count_ = 0Ui64;
	auto start_time = chrono::system_clock::now();
	Move best_move;
	uint32_t iteration_depth = 1;
	int duration = 0;
	string pv_string;
	
	do {
	//for (uint32_t iteration_depth = 1; iteration_depth <= max_depth; iteration_depth++) {
		vector<Move> pv(iteration_depth);
		Score score;
		Loop(iteration_depth, score, pv);

		auto end_time = chrono::system_clock::now();
		chrono::duration<double> elapsed_seconds = end_time - start_time;
		duration = (int)(elapsed_seconds.count() * 1000);
		int nps = elapsed_seconds.count() > 0 ? (int)(node_count_ / elapsed_seconds.count()) : 0;

		cout << "info depth " << iteration_depth << " score " << score.ToString(base_position_.GetActiveColor(), iteration_depth) << " nodes " << node_count_ << " nps " << nps;
		if (score.IsMate()) {
			cout << pv_string << endl;
			break;
		}

		pv_string = " pv";
		for (Move move : pv) {
			pv_string += " " + move.ToString();
		}

		cout << pv_string << endl;
		best_move = pv[0];
		iteration_depth++;
	} while ((duration * 20) < max_duration); //TODO implement game modes like CCRL 40/4

	return best_move;
}

//use a loop (no recursion) to calculate the best move using the AlphaBeta algorithm
void AlphaBeta::Loop(const uint64_t iteration_depth, Score& score, std::vector<Move>& pv) {
	static const int64_t START_SCORE[2] = { Score::WHITE_START_SCORE, Score::BLACK_START_SCORE };

	vector<Position> depth_position(iteration_depth + 1);
	vector<vector<Move>> depth_moves(iteration_depth, vector<Move>(128));
	vector<int> depth_index(iteration_depth);
	vector<Score> depth_score(iteration_depth + 1);
	vector<vector<Move>> depth_variation(iteration_depth + 1);

	depth_position[0] = base_position_;

	int score_depth = 0;
	int moves_depth = -1;

	int depth = 0;
	while (depth >= 0) {
		//evaluate
		if (depth == iteration_depth) {
			Evaluator eval(depth_position[depth]);
			eval.Evaluate(depth_score[depth]);
			score_depth = depth;
			depth--;
		}

		//compare scores
		if (score_depth > depth) {
			if (Evaluator::CompareScore(depth_position[depth].GetActiveColor(), depth_score[score_depth], depth_score[depth]) > 0) {

				//alphabeta-cutoff? (by picking this move, the score would be worse than the current move from the parent's perspective, so the parent would never pick it)
				if (depth > 0) {
					if (Evaluator::CompareScore(depth_position[depth - 1].GetActiveColor(), depth_score[score_depth], depth_score[depth - 1]) < 0) {
						score_depth -= 2;
						depth--;
						moves_depth--;
						continue;
					}
				}

				depth_score[depth] = depth_score[score_depth];
				depth_variation[depth][0] = depth_moves[depth][depth_index[depth]];
				copy(depth_variation[score_depth].begin(), depth_variation[score_depth].end(), depth_variation[depth].begin() + 1);
			}
			score_depth--;
		}

		//generate moves
		if (moves_depth < depth) {
			MoveGenerator move_gen(depth_position[depth]);
			depth_moves[depth].clear();
			move_gen.GenerateMoves(depth_moves[depth]);
			depth_score[depth] = Score(START_SCORE[depth_position[depth].GetActiveColor()]);
			depth_variation[depth] = vector<Move>(iteration_depth - depth);
			depth_index[depth] = 0;
			moves_depth = depth;
			score_depth = depth;
		}
		else {
			depth_index[depth]++;
		}

		//reached end of moves
		if (depth_index[depth] >= depth_moves[depth].size()) {
			moves_depth--;
			depth--;
			continue;
		}

		//apply move
		Position new_position = Position(depth_position[depth]);
		new_position.ApplyMove(depth_moves[depth][depth_index[depth]]);
		node_count_++;
		depth_position[++depth] = new_position;
	}

	pv.assign(depth_variation[0].begin(), depth_variation[0].end());
	score.SetValue(depth_score[0].GetValue());
}
