#pragma once
#include <cstdint>
#include <string>

class Score
{
public:
	static const int64_t BLACK_START_SCORE = 65536;
	static const int64_t WHITE_START_SCORE = -65536;
	static const int64_t BLACK_MATE_SCORE = -32768;
	static const int64_t WHITE_MATE_SCORE = 32768;

	static Score getStartScore(uint8_t color);
	static Score getMateScore(uint8_t color_against, uint32_t depth);
	Score();
	Score(int64_t value);
	~Score();
	int64_t getValue();
	void setValue(int64_t value);
	std::string toString(uint8_t engine_color, uint32_t depth);
	bool isMate();
private:
	int64_t value_;
};

