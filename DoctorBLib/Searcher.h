#pragma once
#include "Position.h"
#include <vector>
#include "Move.h"
#include "Score.h"
#include "HistoryMap.h"

class Searcher
{
public:
	Searcher(const Position& base_position, const HistoryMap& history);
	~Searcher();
	void goDepth(int depth);
	void goTime(uint64_t wtime, uint64_t btime, uint64_t winc, uint64_t binc, uint64_t movestogo);
private:
	uint64_t node_count;
	const Position& base_position_;
	const HistoryMap& history_;
	uint64_t getMaxDuration(uint64_t wtime, uint64_t btime, uint64_t winc, uint64_t binc, uint64_t movestogo);
	bool bookMove();
};

