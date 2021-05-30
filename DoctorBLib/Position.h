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
	void ClearSquare(const Square& square, const Piece& piece);
	bool GetPiece(const Square& square, Piece& piece) const;
	int GetPieceCount(const Piece& piece) const;
	void SetActiveColor(uint8_t color);
	void ToggleActiveColor();
	uint8_t GetActiveColor() const;
	void SetCastlingStatus(int index, bool value);
	bool GetCastlingStatus(int index) const;
	void SetEpSquare(const Square& square);
	void ResetEpSquare();
	bool GetEpSquare(Square& square) const;
	void SetHalfmoveClock(uint16_t value);
	void IncHalfmoveClock();
	uint16_t GetHalfmoveClock();
	std::string ToString() const;
	bool ApplyMove(const Move& move);
	bool GetPieceSquares(const Piece& piece, std::vector<Square>& squares) const;
	bool GetPieceSquare(const Piece& piece, Square& square) const;
	BitBoard GetBitBoard(const Piece& piece) const;
	void RecalculateHashKey();
	uint64_t GetHashKey() const;
private:
	BitBoard bit_boards[12];
	uint8_t active_color;
	uint8_t castling_status_bits;
	boost::optional<Square> ep_square;
	uint16_t halfmove_clock;
	uint64_t hash_key;
};

