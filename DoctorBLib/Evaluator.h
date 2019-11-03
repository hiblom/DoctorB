#pragma once
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
	const Position& position_;
};

