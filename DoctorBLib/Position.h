#pragma once
#include <cstdint>
#include <string>
#include <boost/optional.hpp>
#include <vector>
#include "BitBoard.h"
#include "Piece.h"
#include "Square.h"
#include "Move.h"

class Position
{
public:
	Position();
	~Position();
	void SetPiece(const Square& square, const Piece& piece);
	void ClearSquare(const Square& square);
	bool GetPiece(const Square& square, Piece& piece) const;
	void SetActiveColor(uint8_t color);
	uint8_t GetActiveColor() const;
	void SetCastlingStatus(int index, bool value);
	bool GetCastlingStatus(int index) const;
	void SetEpSquare(const Square& square);
	void ResetEpSquare();
	bool GetEpSquare(Square& square) const;
	void SetHalfmoveClock(uint16_t value);
	uint16_t GetHalfmoveClock();
	void SetFullmoveNumber(uint16_t value);
	uint16_t GetFullmoveNumber();
	std::string ToString() const;
	bool ApplyMove(const Move& move);
	bool GetPieceSquares(const Piece& piece, std::vector<Square>& squares) const;
	BitBoard GetBitBoard(const Piece& piece) const;
private:
	BitBoard bit_boards[12];
	uint8_t active_color;
	bool castling_status[4];
	boost::optional<Square> ep_square;
	uint16_t halfmove_clock;
	uint16_t fullmove_number;
};

