#include "stdafx.h"
#include "Evaluator.h"
#include "Constants.h"

Evaluator::Evaluator(const Position& position) : position_(position) {
}

Evaluator::~Evaluator() {
}

int Evaluator::CompareScore(uint8_t color, Score score_1, Score score_2) {
	if (score_1.GetValue() == score_2.GetValue())
		return 0;

	return ((color == Piece::COLOR_WHITE) == (score_1.GetValue() > score_2.GetValue())) ? 1 : -1;
}

void Evaluator::Evaluate(Score& score) {
	int64_t value = 0;
	for (uint8_t p = 0; p < 12; p++) {
		value += position_.GetBitBoard(Piece(p)).PopCount() * Constants::PIECE_VALUES[p];
	}

	//add random "noise" to prevent DoctorB from playing the same moves over and over
	//noise band = 0..10
	value += (rand() % 11);
	return score.SetValue(value);
}

