#pragma once
#include "SearchAlgorithm.h"

class MiniMax: public SearchAlgorithm
{
public:
	MiniMax(const Position& base_position, HistoryMap& history);
	~MiniMax();
private:
	void Loop(const uint64_t iteration_depth, Score& score, std::vector<Move>& pv);
};

