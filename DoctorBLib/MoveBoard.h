#pragma once
#include "BitBoard.h"
#include <array>

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
	std::array<BitBoard, 64> king_moves;
	std::array<BitBoard, 64> knight_moves;
	std::array<std::array<BitBoard, 64>, 2> pawn_pushes;
	std::array< std::array<BitBoard, 64>, 2> pawn_captures;
	std::array< std::array<BitBoard, 64>, 8> rays;
	std::array<BitBoard, 4> castling_safe_squares;
	std::array<BitBoard, 4> castling_empty_squares;

	MoveBoard();
	void Initialize();
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

