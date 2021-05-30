#pragma once
#include <vector>
#include "Position.h"
#include "Score.h"

class Evaluator
{
public:
	Evaluator(const Position& position);
	void Evaluate(Score& score);
	~Evaluator();
	static int CompareScore(uint8_t color, Score score_1, Score score_2);
private:
	int GetMaterialCount();

	static const int PIECE_SQUARE_START_VALUE[12][64];
	static const int PIECE_SQUARE_END_VALUE[12][64];
	static const int PIECE_START_TOTAL_VALUE;
	const Position& position_;
};