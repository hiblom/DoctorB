#pragma once
#include "Position.h"

class Perft
{
public:
	Perft(const Position& perft_position);
	~Perft();
	void setDepth(int value);
	uint64_t go();
private:
	const Position& position_;
	int depth_;
	uint64_t count(const Position& position, const int remaining_depth) const;
	uint64_t count2(const Position & position, const int remaining_depth) const;
};

