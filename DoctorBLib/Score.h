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

	static Score GetStartScore(uint8_t color);
	static Score GetMateScore(uint8_t color_against, uint32_t depth);
	Score();
	Score(int64_t value);
	int64_t GetValue();
	void SetValue(int64_t value);
	~Score();
	std::string ToString(uint8_t engine_color, uint32_t depth);
	bool IsMate();
private:
	int64_t value_;
};

