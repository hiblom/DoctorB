#pragma once
#include <vector>
#include "Position.h"
#include "HistoryMap.h"
#include "Move.h"
#include "Score.h"

class SearchAlgorithm {
public:
	SearchAlgorithm(const Position& base_position, const HistoryMap& history) : base_position_(base_position), history_(history), node_count_(0) {};
	Move goDepth(uint64_t max_depth);
	Move goTime(uint64_t max_duration);
protected:
	const Position& base_position_;
	const HistoryMap& history_;
	uint64_t node_count_;
	virtual void loop(const uint64_t iteration_depth, Score& score, std::vector<Move>& pv) = 0;
	virtual void afterSearch() {};
	virtual void afterIteration() {};
	virtual int getSearchTimeMultiplier() { return 30; }
};
