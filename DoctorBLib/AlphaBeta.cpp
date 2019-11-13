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

//use a loop (no recursion) to calculate the best move using the AlphaBeta algorithm
void AlphaBeta::Loop(const uint64_t iteration_depth, Score& score, std::vector<Move>& pv) {
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
			
			if (depth_moves[depth].size() == 0)
				if (move_gen.IsCheck(depth_position[depth].GetActiveColor()))
					depth_score[depth] = Score::GetMateScore(depth_position[depth].GetActiveColor(), depth); //mate
				else
					depth_score[depth] = Score(0Ui64); //stale-mate
			else
				depth_score[depth] = Score::GetStartScore(depth_position[depth].GetActiveColor());

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
