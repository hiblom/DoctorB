#include "stdafx.h"
#include "AlphaBetaOrder.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include "MoveGenerator.h"
#include "Evaluator.h"
#include "TranspositionTable.h"
#include "Constants.h"

using namespace std;

AlphaBetaOrder::State::State() : position(Position()), moves(vector<Move>()), move_index(-1), score(Score()), variation(vector<Move>()) {
};

void AlphaBetaOrder::State::generateMoves() {
	moves.clear();
	MoveGenerator move_gen(position);
	move_gen.generateMoves(moves);
}

void AlphaBetaOrder::State::evaluate() {
	Evaluator eval(position);
	eval.evaluate(score);
}

Move& AlphaBetaOrder::State::getActiveMove() {
	return moves[move_index];
}

AlphaBetaOrder::AlphaBetaOrder(const Position& base_position, HistoryMap& history) : SearchAlgorithm(base_position, history) {
}

AlphaBetaOrder::~AlphaBetaOrder() {
}

//use a loop (no recursion) to calculate the best move using the AlphaBeta algorithm with move ordering
void AlphaBetaOrder::loop(const uint64_t iteration_depth, Score& score, std::vector<Move>& pv) {
	vector<State> states(iteration_depth + 1);
	
	Score see_score;
	Score tt_score;
	uint16_t tt_remaining_depth;

	states[0].position = base_position_;

	int current_depth = 0;
	while (current_depth >= 0) {
		State& current_state = states[current_depth];

		//examine three-fold repetition
		if (current_state.move_index == -1) {
			if ((current_depth == 1 || current_depth == 2) && history_.isAtMax(current_state.position.getHashKey())) {
				current_state.score = Score(0);
				current_depth--;
				continue;
			}

			if (TranspositionTable::getInstance().findScore(current_state.position.getHashKey(), tt_score, tt_remaining_depth)) {
				//see if position is in the TT with a score
				if (tt_remaining_depth >= (iteration_depth - current_depth)) {
					current_state.score = tt_score;
					current_depth--;
					continue;
				}
			}
		}

		uint8_t active_color = current_state.position.getActiveColor();
		uint8_t other_color = active_color ^ 1Ui8;

		//evaluate
		if (current_depth == iteration_depth) {
			current_state.evaluate();

			//go to SEE only when it was a capture
			Move last_move = states[current_depth - 1].getActiveMove();
			if (last_move.isCapture()) {
				see_score.setValue(current_state.score.getValue());
				see(current_state.position, last_move.getSquareTo(), see_score);
				//use the "worst" of the scores, current score and see score
				if (Evaluator::compareScore(other_color, current_state.score, see_score) > 0) {
					current_state.score = see_score;
				}
			}

			current_depth--;
			continue;
		}

		//compare scores
		if (current_state.move_index != -1) {
			if (Evaluator::compareScore(active_color, states[current_depth + 1].score, current_state.score) > 0) {
				current_state.score = states[current_depth + 1].score;

				//alphabeta-cutoff: this move is too good, the other side has better options
				if (current_depth > 0) {
					if (Evaluator::compareScore(other_color, current_state.score, states[current_depth - 1].score) < 0) {
						//store the move in the TT. it is obviously a good one!
						TranspositionTable::getInstance().setBestMove(current_state.position.getHashKey(), current_state.getActiveMove());
						
						current_depth--;
						continue;
					}
				}
				
				current_state.variation[0] = current_state.getActiveMove();
				copy(states[current_depth + 1].variation.begin(), states[current_depth + 1].variation.end(), current_state.variation.begin() + 1);
			}
		}

		//generate moves
		if (current_state.move_index == -1) {
			MoveGenerator move_gen(current_state.position);
			current_state.moves.clear();
			move_gen.generateMoves(current_state.moves);
			
			if (current_state.moves.size() == 0) {
				if (move_gen.isCheck()) {
					current_state.score = Score::getMateScore(active_color, current_depth); //mate
				}
				else {
					current_state.score = Score(0Ui64); //stale-mate
				}
			}
			else {
				orderMoves(current_state.position, current_state.moves);
				current_state.score = Score::getStartScore(active_color);
			}
			
			current_state.variation = vector<Move>(iteration_depth - current_depth);
		}

		current_state.move_index++;

		//reached end of moves
		if (current_state.move_index >= current_state.moves.size()) {
			if (current_state.moves.size() > 0) {
				TranspositionTable::getInstance().setBestMove(current_state.position.getHashKey(), current_state.variation[0]);
			}
			TranspositionTable::getInstance().setScore(current_state.position.getHashKey(), current_state.score, static_cast<uint16_t>(iteration_depth - current_depth));

			current_depth--;
			continue;
		}

		//apply move
		Position new_position = Position(current_state.position);
		new_position.applyMove(current_state.getActiveMove());
		node_count_++;

		current_depth++;
		states[current_depth].position = new_position;
		states[current_depth].move_index = -1;
	}

	pv.assign(states[0].variation.begin(), states[0].variation.end());
	score.setValue(states[0].score.getValue());
}

void AlphaBetaOrder::orderMoves(const Position& position, std::vector<Move>& moves) {
	if (moves.size() < 2)
		return;

	int start = 0;

	//if position is in TT, put best move at first position
	Move tt_move;
	if (TranspositionTable::getInstance().findBestMove(position.getHashKey(), tt_move)) {
		std::vector<Move>::iterator pos = find(moves.begin(), moves.end(), tt_move);
		if (pos != moves.begin() && pos != moves.end()) {
			//swap first move and best move
			*pos = moves[0];
			moves[0] = tt_move;
		}
		start = 1;
	}

	//simplesort! put captures and promotions at start
	auto it_start = moves.begin() + start;
	auto it_end = moves.end() - 1;
	while (it_start < it_end) {
		if (!it_end->isCapture() && !it_end->isPromotion()) {
			it_end--;
			continue;
		}

		if (it_start->isCapture() || it_start->isPromotion()) {
			it_start++;
			continue;
		}

		iter_swap(it_start, it_end);
		it_start++;
		it_end--;
	}
}

//we only evaluate captures on the square that was last captured, in order
//of least valuable attacker to most valuable attacker
//until we run out of captures on this square
//then we take the evaluation of the remaining position
//TODO simpler eval
void AlphaBetaOrder::see(const Position& position, const Square& square, Score& score) {
	Move lva_capture;
	Position current_position(position);
	bool captured = false;
	while (true) {
		MoveGenerator move_gen(current_position);
		if (!move_gen.getLvaCapture(square, lva_capture))
			break;
		captured = true;
		current_position.applyMove(lva_capture);
	}
	if (captured) {
		Evaluator eval(current_position);
		eval.evaluate(score);
	}
}

void AlphaBetaOrder::afterSearch() {
	TranspositionTable::getInstance().clear();
}

void AlphaBetaOrder::afterIteration() {
	cout << "hashfull " << TranspositionTable::getInstance().getHashFull() << endl;
}

