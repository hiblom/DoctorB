#pragma once
#include <vector>
#include "Position.h"
#include "Move.h"
#include "Score.h"

class SearchAlgorithm {
public:
	SearchAlgorithm(const Position& base_position) : base_position_(base_position) {};
	virtual Move GoDepth(uint64_t max_depth) = 0;
	virtual Move GoTime(uint64_t max_duration) = 0;
protected:
	const Position& base_position_;
	uint64_t node_count_;
};
