#include "stdafx.h"
#include "AlphaBetaOrder.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include "MoveGenerator.h"
#include "Evaluator.h"
#include "TranspositionTable.h"

using namespace std;

AlphaBetaOrder::AlphaBetaOrder(const Position & base_position) : SearchAlgorithm(base_position) {
}


AlphaBetaOrder::~AlphaBetaOrder() {
}

//use a loop (no recursion) to calculate the best move using the AlphaBeta algorithm with move ordering
void AlphaBetaOrder::Loop(const uint64_t iteration_depth, Score& score, std::vector<Move>& pv) {
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
		//see if position is in the TT with a score
		Score tt_score;
		uint16_t tt_remaining_depth;
		if (depth > 0 && TranspositionTable::GetInstance().FindScore(depth_position[depth].GetHashKey(), tt_score, tt_remaining_depth)) {
			if (tt_remaining_depth >= (iteration_depth - depth)) {
				depth_score[depth] = tt_score;
				score_depth = depth;
				depth--;
			}
		} 
		else if (depth == iteration_depth) {
			//evaluate
			//go to SEE only when it was a capture
			Move last_move = depth_moves[depth - 1][depth_index[depth - 1]];
			if (last_move.IsCapture()) {
				See(depth_position[depth], last_move.GetSquareTo(), depth_score[depth]);
			}
			else {
				Evaluator eval(depth_position[depth]);
				eval.Evaluate(depth_score[depth]);
			}
			score_depth = depth;
			depth--;
		}

		//compare scores
		if (score_depth > depth) {
			bool store_in_tt = depth < 6 || (iteration_depth - score_depth) < 2;
			//store score in TT
			if (store_in_tt)
				TranspositionTable::GetInstance().SetScore(depth_position[depth].GetHashKey(), depth_score[score_depth], (uint16_t)(iteration_depth - score_depth));

			if (Evaluator::CompareScore(depth_position[depth].GetActiveColor(), depth_score[score_depth], depth_score[depth]) > 0) {
				//store move in TT
				if (store_in_tt)
					TranspositionTable::GetInstance().SetBestMove(depth_position[depth].GetHashKey(), depth_moves[depth][depth_index[depth]]);

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
			OrderMoves(depth_position[depth], depth_moves[depth]);
			
			if (depth_moves[depth].size() == 0) {
				if (move_gen.IsCheck())
					depth_score[depth] = Score::GetMateScore(depth_position[depth].GetActiveColor(), depth); //mate
				else
					depth_score[depth] = Score(0Ui64); //stale-mate
			}
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


//if position is in TT, put best move at first position
void AlphaBetaOrder::OrderMoves(const Position& position, std::vector<Move>& moves) {
	if (moves.size() < 2)
		return;

	Move tt_move;
	if (TranspositionTable::GetInstance().FindBestMove(position.GetHashKey(), tt_move)) {
		std::vector<Move>::iterator pos = find(moves.begin(), moves.end(), tt_move);
		if (pos != moves.begin() && pos != moves.end()) {
			//swap first move and best move
			*pos = moves[0];
			moves[0] = tt_move;
		}
	}
}

//for now, the quiescence routine is replaced by a static exchange evaluation
//this means that we only evaluate captures on the square that was last captured, in order
//of least valuable attacker to most valuable attacker
//until we run out of captures on this square
//then we take the evaluation of the remaining position
void AlphaBetaOrder::See(const Position& position, const Square& square, Score& score) {
	Move lva_capture;
	Position current_position(position);
	while (true) {
		MoveGenerator move_gen(current_position);
		if (!move_gen.GetLvaCapture(square, lva_capture))
			break;
		current_position.ApplyMove(lva_capture);
	}
	Evaluator eval(current_position);
	eval.Evaluate(score);
}

void AlphaBetaOrder::AfterSearch() {
	TranspositionTable::GetInstance().Clear();
}

void AlphaBetaOrder::AfterIteration() {
	cout << "hashfull " << TranspositionTable::GetInstance().GetHashFull() << endl;
}
