#pragma once
#include "SearchAlgorithm.h"

class AlphaBeta : public SearchAlgorithm
{
public:
	AlphaBeta(const Position& base_position);
	~AlphaBeta();
	Move GoDepth(uint64_t max_depth);
	Move GoTime(uint64_t max_duration);
private:
	void Loop(const uint64_t iteration_depth, Score& score, std::vector<Move>& pv);

};

