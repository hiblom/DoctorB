#pragma once
#include "BitBoard.h"

class MoveBoard
{
public:
	static const uint8_t DIR_UP = 0Ui8;
	static const uint8_t DIR_DOWN = 1Ui8;
	
	static const uint8_t DIR_RIGHT = 2Ui8;
	static const uint8_t DIR_LEFT = 3Ui8;

	static const uint8_t DIR_UP_RIGHT = 4Ui8;
	static const uint8_t DIR_DOWN_LEFT = 5Ui8;

	static const uint8_t DIR_UP_LEFT = 6Ui8;
	static const uint8_t DIR_DOWN_RIGHT = 7Ui8;

	~MoveBoard();
	static MoveBoard& GetInstance();
	void Initialize();
	MoveBoard(const MoveBoard&) = delete;
	void operator=(const MoveBoard&) = delete;
	BitBoard GetKingMoves(const Square square) const;
	BitBoard GetKnightMoves(const Square square) const;
	BitBoard GetPawnPushes(const Square square, uint8_t color) const;
	BitBoard GetPawnCaptures(const Square square, uint8_t color) const;
	BitBoard GetRay(const Square square, const uint8_t dir) const;
	BitBoard GetCastlingSafeSquares(const int index) const;
	BitBoard GetCastlingEmptySquares(const int index) const;
	BitBoard&(BitBoard::*Forward[2])();
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

