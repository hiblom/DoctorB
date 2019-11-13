#pragma once
#include "SearchAlgorithm.h"
#include <vector>

class AlphaBetaQuiesce : public SearchAlgorithm
{
public:
	AlphaBetaQuiesce(const Position& base_position);
	~AlphaBetaQuiesce();
private:
	void Loop(const uint64_t iteration_depth, Score& score, std::vector<Move>& pv);
	//void Quiesce(const Position& position, Score& score);
	void See(const Position& position, const Square& square, Score& score);
};
