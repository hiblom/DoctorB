#include "stdafx.h"
#include "Searcher.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include "MoveGenerator.h"
#include "Evaluator.h"
#include "AlphaBetaOrder.h"
#include "Options.h"
#include "Polyglot.h"
#include "Globals.h"
#include "Console.h"

using namespace std;

Searcher::Searcher(const Position& base_position, const HistoryMap& history) : base_position_(base_position), history_(history), node_count(0) {
}

void Searcher::goDepth(int depth) {
	if (bookMove())
		return;

	AlphaBetaOrder search_algorithm = AlphaBetaOrder(base_position_, history_);
	Move best_move = search_algorithm.goDepth(depth);
	Console::getInstance() << "bestmove " << best_move.toString() << endl;
}

void Searcher::goTime(uint64_t wtime, uint64_t btime, uint64_t winc, uint64_t binc, uint64_t movestogo) {
	if (bookMove())
		return;

	uint64_t max_duration = getMaxDuration(wtime, btime, winc, binc, movestogo);
	AlphaBetaOrder search_algorithm = AlphaBetaOrder(base_position_, history_);
	Move best_move = search_algorithm.goTime(max_duration);
	Console::getInstance() << "bestmove " << best_move.toString() << endl;
}

Searcher::~Searcher() {
}

uint64_t Searcher::getMaxDuration(uint64_t wtime, uint64_t btime, uint64_t winc, uint64_t binc, uint64_t movestogo) {
	if (movestogo == 0)
		movestogo = 30;

	//be extra careful when moves to go is approaching 1
	int duration_multiplier = movestogo < 5 ? 1 : 2;

	//subtract 50 ms from time left to be on the safe side
	wtime = wtime > 60 ? wtime - 50 : 10;
	btime = btime > 60 ? btime - 50 : 10;

	if (base_position_.getActiveColor() == Piece::COLOR_WHITE)
		return min((wtime + winc * movestogo) * duration_multiplier / movestogo, wtime);
	else
		return min((btime + binc * movestogo) * duration_multiplier / movestogo, btime);
}

bool Searcher::bookMove() {
	if (!Options::OwnBook || Globals::out_of_book)
		return false;

	Move move = Polyglot::getInstance().getMove(base_position_.getHashKey());
	if (move.isValid()) {
		Console::getInstance() << "bestmove " << move.toString() << endl;
		return true;
	}
	else {
		Globals::out_of_book = true;
		return false;
	}
	
}
