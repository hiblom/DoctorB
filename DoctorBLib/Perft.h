#pragma once
#include "Position.h"

class Perft
{
public:
	Perft(const Position& perft_position);
	~Perft();
	void SetDepth(int value);
	uint64_t Go();
private:
	const Position& position_;
	int depth_;
	uint64_t Count(const Position& position, const int remaining_depth) const;
};

