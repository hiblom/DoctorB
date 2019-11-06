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
	static const int PIECE_SQUARE_VALUE[12][64];
	const Position& position_;
};