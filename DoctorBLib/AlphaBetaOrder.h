#pragma once
#include "SearchAlgorithm.h"

class AlphaBetaOrder : public SearchAlgorithm
{
public:
	AlphaBetaOrder(const Position& base_position, HistoryMap& history);
	~AlphaBetaOrder();
private:
	void loop(const uint64_t iteration_depth, Score& score, std::vector<Move>& pv);
	void orderMoves(const Position& position, std::vector<Move>& moves);
	void see(const Position & position, const Square & square, Score & score);
	void afterSearch();
	void afterIteration();
	int getSearchTimeMultiplier() { return 15; }
};

