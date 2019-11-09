#pragma once
#include <vector>
#include "SearchAlgorithm.h"
#include "Position.h"
#include "Move.h"
#include "Score.h"

class MiniMax: public SearchAlgorithm
{
public:
	MiniMax(const Position& base_position);
	~MiniMax();
	Move GoDepth(uint64_t max_depth);
	Move GoTime(uint64_t max_duration);
private:
	void Loop(const uint32_t iteration_depth, Score& score, std::vector<Move>& pv);
};

