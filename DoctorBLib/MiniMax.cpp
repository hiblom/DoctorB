#include "stdafx.h"
#include "MiniMax.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include "MoveGenerator.h"
#include "Evaluator.h"

using namespace std;

MiniMax::MiniMax(const Position& base_position, HistoryMap& history) : SearchAlgorithm(base_position, history) {
}

MiniMax::~MiniMax() {
}

//use a loop (no recursion) to calculate the best move using the MiniMax algorithm
void MiniMax::Loop(const uint64_t iteration_depth, Score& score, std::vector<Move>& pv) {
	vector<Position> position_stack(iteration_depth + 1);
	position_stack[0] = base_position_;

	vector<vector<Move>> moves_stack(iteration_depth + 1, vector<Move>(128));
	vector<int> index_stack(iteration_depth + 1);

	vector<Score> score_stack(iteration_depth + 1);
	vector<vector<Move>> variation_stack(iteration_depth + 1);

	Score pos_score;

	int score_depth = 0;
	int moves_depth = -1;
	int depth = 0;
	while (depth >= 0) {
		//evaluate
		if (depth == iteration_depth) {
			Evaluator eval(position_stack[depth]);
			eval.Evaluate(score_stack[depth]);
			score_depth = depth;
			depth--;
		}

		//best move(s)?
		if (score_depth > depth) {
			if (Evaluator::CompareScore(position_stack[depth].GetActiveColor(), score_stack[score_depth], score_stack[depth]) > 0) {
				score_stack[depth] = score_stack[score_depth];
				variation_stack[depth][0] = moves_stack[depth][index_stack[depth]];
				copy(variation_stack[score_depth].begin(), variation_stack[score_depth].end(), variation_stack[depth].begin() + 1);
			}
			score_depth = depth;
		}

		//generate moves
		if (moves_depth < depth) {
			MoveGenerator move_gen(position_stack[depth]);
			moves_stack[depth].clear();
			move_gen.GenerateMoves(moves_stack[depth]);

			if (moves_stack[depth].size() == 0)
				if (move_gen.IsCheck())
					score_stack[depth] = Score::GetMateScore(position_stack[depth].GetActiveColor(), depth); //mate
				else
					score_stack[depth] = Score(0Ui64); //stale-mate
			else
				score_stack[depth] = Score::GetStartScore(position_stack[depth].GetActiveColor());
			
			variation_stack[depth] = vector<Move>(iteration_depth - depth);
			score_depth = depth;
			index_stack[depth] = 0;
			moves_depth = depth;
		}
		else {
			index_stack[depth]++;
		}

		//reached end of moves
		if (index_stack[depth] >= moves_stack[depth].size()) {
			moves_depth--;
			depth--;
			continue;
		}

		//apply move
		Position new_position = Position(position_stack[depth]);
		new_position.ApplyMove(moves_stack[depth][index_stack[depth]]);
		node_count_++;
		position_stack[++depth] = new_position;
	}

	pv.assign(variation_stack[0].begin(), variation_stack[0].end());
	score.SetValue(score_stack[0].GetValue());
}
