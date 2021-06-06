#include "stdafx.h"
#include "Score.h"
#include <array>
#include "Piece.h"

using namespace std;

Score Score::getStartScore(uint8_t color) {
	static const array<int64_t, 2> START_SCORE { { Score::BLACK_START_SCORE, Score::WHITE_START_SCORE } };
	return Score(START_SCORE[color]);
}

Score Score::getMateScore(uint8_t color_against, uint32_t depth) {
	static const array<int, 2> mult { { -1, 1 } };
	static const array<int64_t, 2> START_SCORE{ { Score::BLACK_START_SCORE, Score::WHITE_START_SCORE } };
	return Score(START_SCORE[color_against] + mult[color_against] *depth);
}

Score::Score() {
	value_ = 0i64;
}

Score::Score(int64_t value) {
	value_ = value;
}

int64_t Score::getValue() {
	return value_;
}

void Score::setValue(int64_t value) {
	value_ = value;
}

Score::~Score() {
}

string Score::toString(uint8_t engine_color, uint32_t depth) {
	int mult = (engine_color == Piece::COLOR_BLACK) ? -1 : 1;

	if (!isMate())
		return "cp " + to_string(mult * value_);

	mult *= (value_ > 0) ? 1 : -1;

	int moves_to_mate = (depth - 1) / 2;
	return "mate " + to_string(mult * moves_to_mate);
}

bool Score::isMate() {
	return (value_ <= BLACK_MATE_SCORE || value_ >= WHITE_MATE_SCORE);
}
