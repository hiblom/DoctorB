#include "stdafx.h"
#include "MiniMax.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <stack>
#include "MoveGenerator.h"
#include "Evaluator.h"

using namespace std;

MiniMax::MiniMax(const Position& base_position) : SearchAlgorithm(base_position) {
}

MiniMax::~MiniMax() {
}

Move MiniMax::GoDepth(uint64_t max_depth) {
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

		pv_string = " pv";
		for (Move move: pv) {
			pv_string += " " + move.ToString();
		}

		cout << pv_string << endl;
		best_move = pv[0];

		if (score.IsMate())
			break;
	}

	return best_move;
}

Move MiniMax::GoTime(uint64_t max_duration) {
	node_count_ = 0Ui64;

	auto start_time = chrono::system_clock::now();
	Move best_move;
	uint32_t iteration_depth = 1;
	int duration = 0;
	string pv_string;

	do {
		vector<Move> pv(iteration_depth);
		Score score;
		Loop(iteration_depth, score, pv);

		auto end_time = chrono::system_clock::now();
		chrono::duration<double> elapsed_seconds = end_time - start_time;
		duration = (int)(elapsed_seconds.count() * 1000);

		int nps = elapsed_seconds.count() > 0 ? (int)(node_count_ / elapsed_seconds.count()) : 0;

		cout << "info depth " << iteration_depth << " score " << score.ToString(base_position_.GetActiveColor(), iteration_depth) << " nodes " << node_count_ << " nps " << nps;

		pv_string = " pv";
		for (int i = iteration_depth; i > 0; i--) {
			pv_string += " " + pv[i].ToString();
		}
		cout << pv_string << endl;

		best_move = pv[iteration_depth];
		if (score.IsMate())
			break;

		iteration_depth++;
	} while ((duration * 30) < max_duration); //TODO implement game modes like CCRL 40/4


	return best_move;
}

//use a loop (no recursion) to calculate the best move using the MiniMax algorithm
void MiniMax::Loop(const uint32_t iteration_depth, Score& score, std::vector<Move>& pv) {
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
				if (move_gen.IsCheck(position_stack[depth].GetActiveColor()))
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
