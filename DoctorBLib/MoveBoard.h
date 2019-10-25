#pragma once
#include "BitBoard.h"

class MoveBoard
{
public:
	static const int DIR_UP = 0;
	static const int DIR_UP_RIGHT = 1;
	static const int DIR_RIGHT = 2;
	static const int DIR_DOWN_RIGHT = 3;
	static const int DIR_DOWN = 4;
	static const int DIR_DOWN_LEFT = 5;
	static const int DIR_LEFT = 6;
	static const int DIR_UP_LEFT = 7;

	~MoveBoard();
	static MoveBoard& GetInstance();
	void Initialize();
	MoveBoard(const MoveBoard&) = delete;
	void operator=(const MoveBoard&) = delete;
	BitBoard GetKingMoves(const Square square) const;
	BitBoard GetKnightMoves(const Square square) const;
	BitBoard GetPawnPushes(const Square square, uint8_t color) const;
	BitBoard GetPawnCaptures(const Square square, uint8_t color) const;
	BitBoard GetRay(const Square square, const int dir) const;
	BitBoard GetCastlingSafeSquares(const int index) const;
	BitBoard GetCastlingEmptySquares(const int index) const;
private:
	BitBoard king_moves[64];
	BitBoard knight_moves[64];
	BitBoard pawn_pushes[2][64];
	BitBoard pawn_captures[2][64];
	BitBoard rays[8][64];
	BitBoard castling_safe_squares[4];
	BitBoard castling_empty_squares[4];
	MoveBoard();
	void InitializeKingMoves();
	void InitializeKnightMoves();
	void InitializePawnPushes();
	void InitializePawnCaptures();
	void InitializeRays();
	void InitializeRayUp();
	void InitializeRayUpRight();
	void InitializeRayRight();
	void InitializeRayDownRight();
	void InitializeRayDown();
	void InitializeRayDownLeft();
	void InitializeRayLeft();
	void InitializeRayUpLeft();
	void InitializeCastlingSquares();
};

