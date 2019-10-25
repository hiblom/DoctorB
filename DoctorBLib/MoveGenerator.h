#pragma once
#include <vector>
#include "Position.h"
#include "Move.h"

class MoveGenerator {
public:
	MoveGenerator(const Position& generator_position);
	~MoveGenerator();
	void GenerateMoves(std::vector<Move>& moves) const;
	void GenerateKingMoves(std::vector<Move>& moves) const;
	void GenerateKnightMoves(std::vector<Move>& moves) const;
	void GeneratePawnMoves(std::vector<Move>& moves) const;
	void GenerateRookMoves(std::vector<Move>& moves) const;
	void GenerateBishopMoves(std::vector<Move>& moves) const;
	void GenerateQueenMoves(std::vector<Move>& moves) const;
private:
	const Position& position;
	BitBoard active_board;
	BitBoard inactive_board;
	BitBoard combined_board;
	void GenerateRayMoves(const Square& from_square, const int dir, bool(BitBoard::*find_nearest_square)(Square&) const, std::vector<Move>& moves) const;
};

