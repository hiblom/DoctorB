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
	void GoTime(uint64_t wtime, uint64_t btime, uint64_t winc, uint64_t binc, uint64_t movestogo);
	~Searcher();
private:
	uint64_t node_count;
	const Position base_position_;
	uint64_t GetMaxDuration(uint64_t wtime, uint64_t btime, uint64_t winc, uint64_t binc, uint64_t movestogo);
};

