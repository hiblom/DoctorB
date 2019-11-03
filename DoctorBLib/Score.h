#pragma once
#include <cstdint>

class Score
{
public:
	Score();
	Score(int64_t value);
	int64_t GetValue();
	void SetValue(int64_t value);
	~Score();
private:
	int64_t value_;
};

