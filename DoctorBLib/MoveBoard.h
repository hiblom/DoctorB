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
	static MoveBoard& getInstance();
	MoveBoard(const MoveBoard&) = delete;
	void operator=(const MoveBoard&) = delete;
	BitBoard getKingMoves(const Square square) const;
	BitBoard getKnightMoves(const Square square) const;
	BitBoard getPawnPushes(const Square square, uint8_t color) const;
	BitBoard getPawnCaptures(const Square square, uint8_t color) const;
	BitBoard getRay(const Square square, const uint8_t dir) const;
	BitBoard getCastlingSafeSquares(const int index) const;
	BitBoard getCastlingEmptySquares(const int index) const;
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
	void initialize();
	void initializeKingMoves();
	void initializeKnightMoves();
	void initializePawnPushes();
	void initializePawnCaptures();
	void initializeRays();
	void initializeRayUp();
	void initializeRayUpRight();
	void initializeRayRight();
	void initializeRayDownRight();
	void initializeRayDown();
	void initializeRayDownLeft();
	void initializeRayLeft();
	void initializeRayUpLeft();
	void initializeCastlingSquares();
};

