#include "stdafx.h"
#include "AlphaBetaQuiesce.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include "MoveGenerator.h"
#include "Evaluator.h"

using namespace std;

AlphaBetaQuiesce::AlphaBetaQuiesce(const Position& base_position) : SearchAlgorithm(base_position) {
}

AlphaBetaQuiesce::~AlphaBetaQuiesce() {
}

void AlphaBetaQuiesce::Loop(const uint64_t iteration_depth, Score& score, std::vector<Move>& pv) {
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
			//go to See only when it was a capture
			//for now we check board at depth -1 to see if there was a piece at the square
			//TODO put capture info in move

			Square square = depth_moves[depth - 1][depth_index[depth - 1]].GetSquareTo(); //get last square moved to
			Piece dummy_piece;
			if (depth_position[depth - 1].GetPiece(square, dummy_piece)) {
				See(depth_position[depth], square, depth_score[depth]);
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


//for now, the quiescence routine is replaced by a static exchange evaluation
//this means that we only evaluate captures on the square that was last captured, in order
//of least valuable attacker to most valuable attacker
//until we run out of captures on this square
//then we take the evaluation of the remaining position
void AlphaBetaQuiesce::See(const Position& position, const Square& square, Score& score) {
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


//keep doing captures until the board reaches a "quiet" state, then return the score
//TODO weave this into the regular loop (?)
/*
void AlphaBetaQuiesce::Quiesce(const Position& quiesce_position, Score& score) {
	//we are not interested in variants here

	quiesce_depth_position[0] = quiesce_position;

	int score_depth = 0;
	int moves_depth = -1;
	int depth = 0;
	while (depth >= 0) {
		//when we reach max depth for quiesce, we evaluate anyway
		if (depth == MAX_QUIESCE_DEPTH) {
			Evaluator eval(quiesce_depth_position[depth]);
			eval.Evaluate(quiesce_depth_score[depth]);
			score_depth = depth;
			depth--;
		}

		//compare scores
		if (score_depth > depth) {
			if (Evaluator::CompareScore(quiesce_depth_position[depth].GetActiveColor(), quiesce_depth_score[score_depth], quiesce_depth_score[depth]) > 0) {

				//alphabeta-cutoff? (by picking this move, the score would be worse than the current move from the parent's perspective, so the parent would never pick it)
				if (depth > 0) {
					if (Evaluator::CompareScore(quiesce_depth_position[depth - 1].GetActiveColor(), quiesce_depth_score[score_depth], quiesce_depth_score[depth - 1]) < 0) {
						score_depth -= 2;
						depth--;
						moves_depth--;
						continue;
					}
				}

				quiesce_depth_score[depth] = quiesce_depth_score[score_depth];
			}
			score_depth--;
		}

		//generate captures only...
		//TODO also generate promotions, checks
		if (moves_depth < depth) {
			MoveGenerator move_gen(quiesce_depth_position[depth], true);
			quiesce_depth_captures[depth].clear();
			move_gen.GenerateMoves(quiesce_depth_captures[depth]);

			if (quiesce_depth_captures[depth].size() == 0) {
				//we have run out of captures. evaluate
				Evaluator eval(quiesce_depth_position[depth]);
				eval.Evaluate(quiesce_depth_score[depth]);
				score_depth = depth;
				depth--;
				continue;
			}

			quiesce_depth_score[depth] = Score::GetStartScore(quiesce_depth_position[depth].GetActiveColor());
			quiesce_depth_index[depth] = 0;
			moves_depth = depth;
			score_depth = depth;
		}
		else {
			quiesce_depth_index[depth]++;
		}

		//reached end of moves
		if (quiesce_depth_index[depth] >= quiesce_depth_captures[depth].size()) {
			moves_depth--;
			depth--;
			continue;
		}

		//apply move
		Position new_position = Position(quiesce_depth_position[depth]);
		new_position.ApplyMove(quiesce_depth_captures[depth][quiesce_depth_index[depth]]);
		node_count_++;
		quiesce_depth_position[++depth] = new_position;
	}

	score.SetValue(quiesce_depth_score[0].GetValue());
}
*/