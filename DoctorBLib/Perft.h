#pragma once
#include <vector>
#include "Position.h"
#include "MoveGenerator.h"

class Perft
{
public:
	Perft(const Position& perft_position);
	~Perft();
	void setDepth(int value);
	uint64_t go() const;
private:
	struct State {
		Position position;
		std::vector<Move> moves;
		int move_index;
		State() : position(Position()), moves(std::vector<Move>()), move_index(-1) {};
		void generateMoves() {
			moves.clear();
			MoveGenerator move_gen(position);
			move_gen.generateMoves(moves);
		}
	};

	const Position& position_;
	int depth_;
};

