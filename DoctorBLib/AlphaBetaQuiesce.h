#pragma once
#include "SearchAlgorithm.h"
#include <vector>

class AlphaBetaQuiesce : public SearchAlgorithm
{
public:
	AlphaBetaQuiesce(const Position& base_position, HistoryMap& history);
	~AlphaBetaQuiesce();
private:
	void loop(const uint64_t iteration_depth, Score& score, std::vector<Move>& pv);
	void quiesce(const Position& start_position, Score& score);
	void see(const Position& position, const Square& square, Score& score);
	void orderMoves(const Position& position, std::vector<Move>& moves);

	const int MAX_QUIESCE_DEPTH { 6 };
};
