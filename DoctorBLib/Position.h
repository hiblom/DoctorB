#pragma once
#include <cstdint>
#include <string>
#include <array>
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
	void setPiece(const Square& square, const Piece& piece);
	void clearSquare(const Square& square);
	void clearSquare(const Square& square, const Piece& piece);
	bool getPiece(const Square& square, Piece& piece) const;
	int getPieceCount(const Piece& piece) const;
	void setActiveColor(uint8_t color);
	void toggleActiveColor();
	uint8_t getActiveColor() const;
	void setCastlingStatus(int index, bool value);
	bool getCastlingStatus(int index) const;
	void setEpSquare(const Square& square);
	void resetEpSquare();
	bool getEpSquare(Square& square) const;
	void setHalfmoveClock(uint16_t value);
	void incHalfmoveClock();
	uint16_t getHalfmoveClock();
	std::string toString() const;
	bool applyMove(const Move& move);
	bool getPieceSquares(const Piece& piece, std::vector<Square>& squares) const;
	bool getPieceSquare(const Piece& piece, Square& square) const;
	BitBoard getBitBoard(const Piece& piece) const;
	void recalculateHashKey();
	uint64_t getHashKey() const;
	uint16_t getHalfmoveClock() const;
private:
	std::array<BitBoard, 12> bit_boards_;
	uint8_t active_color_;
	uint8_t castling_status_bits_;
	boost::optional<Square> ep_square_;
	uint16_t halfmove_clock_;
	uint64_t hash_key_;
};

