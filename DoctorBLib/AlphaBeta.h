#pragma once
#include "SearchAlgorithm.h"

class AlphaBeta : public SearchAlgorithm
{
public:
	AlphaBeta(const Position& base_position, HistoryMap& history);
	~AlphaBeta();
private:
	void Loop(const uint64_t iteration_depth, Score& score, std::vector<Move>& pv);
};

