#pragma once
#include "Position.h"
#include <vector>
#include "Move.h"
#include "Score.h"

class MiniMax
{
public:
	MiniMax(Position base_position);
	~MiniMax();
	Move GoDepth(uint32_t max_depth);
	Move GoTime(uint64_t max_duration);
private:
	uint64_t node_count_;
	const Position base_position_;
	void Recurse(const uint32_t remaining_depth, const Position & position, Score & score, std::vector<Move>& pv);
	void Loop(const uint32_t iteration_depth, Score& score, std::vector<Move>& pv);
};

