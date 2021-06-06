#pragma once
#include "SearchAlgorithm.h"
#include <vector>

class AlphaBetaQuiesce : public SearchAlgorithm
{
public:
	AlphaBetaQuiesce(const Position& base_position, HistoryMap& history);
	~AlphaBetaQuiesce();
private:
	void Loop(const uint64_t iteration_depth, Score& score, std::vector<Move>& pv);
	void Quiesce(const Position& start_position, Score& score);
	void See(const Position& position, const Square& square, Score& score);
	void OrderMoves(const Position& position, std::vector<Move>& moves);

	const int MAX_QUIESCE_DEPTH { 6 };
};
