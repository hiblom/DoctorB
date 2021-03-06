#pragma once
#include "SearchAlgorithm.h"
#include <vector>
#include "HistoryMap.h"
#include "Position.h"
#include "Move.h"
#include "Square.h"

class AlphaBetaOrder : public SearchAlgorithm
{
public:
	AlphaBetaOrder(const Position& base_position, const HistoryMap& history);
	~AlphaBetaOrder();
private:
	struct State {
		Position position;
		std::vector<Move> moves;
		int move_index;
		Score score;
		std::vector<Move> variation;
		State();
		void generateMoves();
		void evaluate();
		Move& getActiveMove();
	};

	void loop(const uint64_t iteration_depth, Score& score, std::vector<Move>& pv);
	void orderMoves(const Position& position, std::vector<Move>& moves);
	void see(const Position& position, const Square& square, Score& score);
	void afterSearch();
	void afterIteration();
	int getSearchTimeMultiplier() { return 12; } //expected factor the next iteration takes over the last one
};

