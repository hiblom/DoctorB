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
	void GoTime(int wtime, int btime, int winc, int binc);
	~Searcher();
private:
	uint64_t node_count;
	const Position base_position_;
	Move MinMaxDepth(uint32_t max_depth);
	Move MinMaxTime(int wtime, int btime, int winc, int binc);
	void MinMaxRecurse(const uint32_t remaining_depth, const Position& position, Score& score, std::vector<Move>& pv);
};

