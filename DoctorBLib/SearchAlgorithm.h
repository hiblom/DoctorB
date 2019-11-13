#pragma once
#include <vector>
#include "Position.h"
#include "Move.h"
#include "Score.h"

class SearchAlgorithm {
public:
	SearchAlgorithm(const Position& base_position) : base_position_(base_position) {};
	Move GoDepth(uint64_t max_depth);
	Move GoTime(uint64_t max_duration);
protected:
	const Position& base_position_;
	uint64_t node_count_;
	virtual void Loop(const uint64_t iteration_depth, Score& score, std::vector<Move>& pv) = 0;
	virtual void AfterSearch() {};
};
