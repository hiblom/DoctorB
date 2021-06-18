#pragma once
#include <vector>
#include <array>
#include "Position.h"
#include "Score.h"

class Evaluator
{
public:
	Evaluator(const Position& position);
	~Evaluator();
	void evaluate(Score& score);
	static int compareScore(uint8_t color, Score score_1, Score score_2);
private:
	int getMaterialCount();

	static const std::array<std::array<int, 64>, 12> PIECE_SQUARE_START_VALUE;
	static const std::array<std::array<int, 64>, 12> PIECE_SQUARE_END_VALUE;
	static const int PIECE_START_TOTAL_VALUE;
	static const std::array<int, 2> TEMPO_PENALTY;
	const Position& position_;
};