#pragma once
#include "SearchAlgorithm.h"

class AlphaBetaOrder : public SearchAlgorithm
{
public:
	AlphaBetaOrder(const Position& base_position);
	~AlphaBetaOrder();
private:
	void Loop(const uint64_t iteration_depth, Score& score, std::vector<Move>& pv);
	void OrderMoves(const Position& position, std::vector<Move>& moves);
	void See(const Position & position, const Square & square, Score & score);
	void AfterSearch();
	void AfterIteration();
	int GetSearchTimeMultiplier() { return 12; }
};

