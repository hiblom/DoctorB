#pragma once
#include "Position.h"
#include <vector>
#include "Move.h"
#include "Score.h"

class Searcher
{
public:
	Searcher(Position base_position);
	void GoDepth(int depth);
	~Searcher();
private:
	uint64_t node_count;
	const Position base_position_;
	Move MinMax(uint32_t max_depth);
	void MinMaxRecurse(const uint32_t remaining_depth, const Position& position, Score& score, std::vector<Move>& pv);
};

