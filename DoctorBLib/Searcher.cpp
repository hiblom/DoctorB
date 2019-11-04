#include "stdafx.h"
#include "Searcher.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include "MoveGenerator.h"
#include "Evaluator.h"
#include "MiniMax.h"

using namespace std;

Searcher::Searcher(Position base_position) : base_position_(base_position) {
	node_count = 0;
}

void Searcher::GoDepth(int depth) {
	MiniMax search_algorithm = MiniMax(base_position_);
	Move best_move = search_algorithm.GoDepth(depth);
	cout << "bestmove " << best_move.ToString() << endl;
}

void Searcher::GoTime(uint64_t wtime, uint64_t btime, uint64_t winc, uint64_t binc, uint64_t movestogo) {
	int max_duration = GetMaxDuration(wtime, btime, winc, binc, movestogo);
	MiniMax search_algorithm = MiniMax(base_position_);
	Move best_move = search_algorithm.GoTime(max_duration);
	//Move best_move = AlphaBetaTime(max_duration);
	cout << "bestmove " << best_move.ToString() << endl;
}

Searcher::~Searcher() {
}


Move Searcher::AlphaBetaTime(uint64_t max_duration) {
	node_count = 0Ui64;

	auto start_time = chrono::system_clock::now();
	Move best_move;
	uint32_t iteration_depth = 1;
	int duration = 0;
	string pv_string;

	do {
		vector<Move> pv(iteration_depth + 1);
		Score score;
		AlphaBetaTop(iteration_depth, base_position_, score, pv);

		auto end_time = chrono::system_clock::now();
		chrono::duration<double> elapsed_seconds = end_time - start_time;
		duration = (int)(elapsed_seconds.count() * 1000);

		int nps = elapsed_seconds.count() > 0 ? (int)(node_count / elapsed_seconds.count()) : 0;

		cout << "info depth " << iteration_depth << " score " << score.ToString(base_position_.GetActiveColor(), iteration_depth) << " nodes " << node_count << " nps " << nps;
		if (score.IsMate()) {
			cout << pv_string << endl;
			break;
		}

		pv_string = " pv";
		for (int i = iteration_depth; i > 0; i--) {
			pv_string += " " + pv[i].ToString();
		}
		cout << pv_string << endl;

		best_move = pv[iteration_depth];

		iteration_depth++;
	} while ((duration * 40) < max_duration);


	return best_move;
}

void Searcher::AlphaBetaTop(const uint32_t remaining_depth, const Position& position, Score& score, std::vector<Move>& pv) {
	//top layer of alfa beta search, there can be no cutoff here
	static const int64_t START_SCORE[2] = { Score::WHITE_START_SCORE, Score::BLACK_START_SCORE };

	uint8_t active_color = position.GetActiveColor();
	uint8_t inactive_color = active_color ^ 1Ui8;

	vector<Move> moves;
	vector<Move> child_variant(remaining_depth + 1);
	MoveGenerator move_generator(position);
	move_generator.GenerateMoves(moves);
	Score best_score;
	Move best_move;

	Score local_score = Score(START_SCORE[active_color]);
	Score best_white = Score(Score::WHITE_START_SCORE);
	Score best_black = Score(Score::BLACK_START_SCORE);

	for (Move move : moves) {
		Position new_position(position);
		new_position.ApplyMove(move);
		node_count++;
		Score new_score;
		AlphaBetaRecurse(remaining_depth - 1, new_position, best_white, best_black, new_score, child_variant);

		if (Evaluator::CompareScore(active_color, new_score, local_score) > 0) {
			local_score.SetValue(new_score.GetValue());
			score.SetValue(new_score.GetValue());
			if (score.IsMate()) {
				cout << "mate" << endl;
			}

			pv[remaining_depth] = move;
			//variant array has the moves in reverse order; pv[1] is the last move; pv[remaining_depth] is the first move
			//element at [0] is not used
			//copy moves after the current move
			if (remaining_depth > 1)
				copy(child_variant.begin() + 1, child_variant.begin() + remaining_depth, pv.begin() + 1);

			//update current node max
			if (active_color == Piece::COLOR_WHITE) {
				if (Evaluator::CompareScore(active_color, new_score, best_white) > 0)
					best_white = new_score;
			}
			else {
				if (Evaluator::CompareScore(active_color, new_score, best_black) > 0)
					best_black = new_score;
			}
		}
	}
}

void Searcher::AlphaBetaRecurse(const uint32_t remaining_depth, const Position& position, Score best_white, Score best_black, Score& score, std::vector<Move>& pv) {
	static const int64_t START_SCORE[2] = { Score::WHITE_START_SCORE, Score::BLACK_START_SCORE };

	if (remaining_depth == 0) {
		Evaluator eval(position);
		eval.Evaluate(score);
		return;
	}

	uint8_t active_color = position.GetActiveColor();
	uint8_t inactive_color = active_color ^ 1Ui8;

	Score cutoff_score = inactive_color == Piece::COLOR_WHITE ? best_white : best_black;

	vector<Move> moves;
	vector<Move> child_variant(remaining_depth + 1);
	MoveGenerator move_generator(position);
	move_generator.GenerateMoves(moves);
	Score best_score;
	Move best_move;

	Score local_score = Score(START_SCORE[active_color]);

	for (Move move : moves) {
		Position new_position(position);
		new_position.ApplyMove(move);
		node_count++;
		Score new_score;
		AlphaBetaRecurse(remaining_depth - 1, new_position, best_white, best_black, new_score, child_variant);

		//cutoff?
		if (Evaluator::CompareScore(inactive_color, new_score, cutoff_score) <= 0)
			return;

		if (Evaluator::CompareScore(active_color, new_score, local_score) > 0) {
			local_score.SetValue(new_score.GetValue());
			score.SetValue(new_score.GetValue());
			if (score.IsMate()) {
				cout << "mate" << endl;
			}

			pv[remaining_depth] = move;
			//variant array has the moves in reverse order; pv[1] is the last move; pv[remaining_depth] is the first move
			//element at [0] is not used
			//copy moves after the current move
			if (remaining_depth > 1)
				copy(child_variant.begin() + 1, child_variant.begin() + remaining_depth, pv.begin() + 1);

			//update current node max
			if (active_color == Piece::COLOR_WHITE) {
				if (Evaluator::CompareScore(active_color, new_score, best_white) > 0)
					best_white = new_score;
			}
			else {
				if (Evaluator::CompareScore(active_color, new_score, best_black) > 0)
					best_black = new_score;
			}

		}
	}
}

uint64_t Searcher::GetMaxDuration(uint64_t wtime, uint64_t btime, uint64_t winc, uint64_t binc, uint64_t movestogo) {
	if (movestogo == 0)
		movestogo = 40;

	//be extra careful when moves to go is approaching 1
	int duration_multiplier = 20;
	if (movestogo == 1)
		duration_multiplier = 80;
	else if (movestogo < 5)
		duration_multiplier = 40;

	//subtract 50 ms from time left to be on the safe side
	wtime = wtime > 60 ? wtime - 50 : 10;
	btime = btime > 60 ? btime - 50 : 10;

	return (base_position_.GetActiveColor() == Piece::COLOR_WHITE) ? (wtime / movestogo + winc) : (btime / movestogo + binc);
}
