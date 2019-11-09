#pragma once
#include "SearchAlgorithm.h"

class AlphaBetaOrder : public SearchAlgorithm
{
public:
	AlphaBetaOrder(const Position& base_position);
	~AlphaBetaOrder();
	Move GoDepth(uint64_t max_depth);
	Move GoTime(uint64_t max_duration);
private:
	void Loop(const uint64_t iteration_depth, Score& score, std::vector<Move>& pv);
	void OrderMoves(const Position& position, std::vector<Move>& moves);
};

