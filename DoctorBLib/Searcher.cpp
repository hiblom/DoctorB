#include "stdafx.h"
#include "Searcher.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include "MoveGenerator.h"
#include "Evaluator.h"
#include "AlphaBetaOrder.h"
#include "AlphaBetaQuiesce.h"
#include "Options.h"
#include "Polyglot.h"
#include "Globals.h"

using namespace std;

Searcher::Searcher(const Position& base_position, HistoryMap& history) : base_position_(base_position), history_(history), node_count(0) {
}

void Searcher::GoDepth(int depth) {
	if (BookMove())
		return;

	AlphaBetaOrder search_algorithm = AlphaBetaOrder(base_position_, history_);
	Move best_move = search_algorithm.GoDepth(depth);
	cout << "bestmove " << best_move.ToString() << endl;
}

void Searcher::GoTime(uint64_t wtime, uint64_t btime, uint64_t winc, uint64_t binc, uint64_t movestogo) {
	if (BookMove())
		return;

	uint64_t max_duration = GetMaxDuration(wtime, btime, winc, binc, movestogo);
	AlphaBetaOrder search_algorithm = AlphaBetaOrder(base_position_, history_);
	Move best_move = search_algorithm.GoTime(max_duration);
	cout << "bestmove " << best_move.ToString() << endl;
}

Searcher::~Searcher() {
}

uint64_t Searcher::GetMaxDuration(uint64_t wtime, uint64_t btime, uint64_t winc, uint64_t binc, uint64_t movestogo) {
	if (movestogo == 0)
		movestogo = 30;

	//be extra careful when moves to go is approaching 1
	int duration_multiplier = movestogo < 5 ? 1 : 2;

	//subtract 50 ms from time left to be on the safe side
	wtime = wtime > 60 ? wtime - 50 : 10;
	btime = btime > 60 ? btime - 50 : 10;

	if (base_position_.GetActiveColor() == Piece::COLOR_WHITE)
		return min((wtime + winc * movestogo) * duration_multiplier / movestogo, wtime);
	else
		return min((btime + binc * movestogo) * duration_multiplier / movestogo, btime);
}

bool Searcher::BookMove() {
	if (!Options::OwnBook || Globals::out_of_book)
		return false;

	Move move = Polyglot::get_instance().get_move(base_position_.GetHashKey());
	if (move.IsValid()) {
		cout << "bestmove " << move.ToString() << endl;
		return true;
	}
	else {
		Globals::out_of_book = true;
		return false;
	}
	
}
