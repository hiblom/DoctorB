#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "Square.h"

class BitBoard
{
public:
	BitBoard();
	BitBoard(uint64_t bit_board_value);
	~BitBoard();
	bool empty();
	bool notEmpty();
	uint8_t popCount() const;
	BitBoard& set(uint8_t square_value);
	BitBoard& clear(uint8_t square_value);
	bool check(uint8_t square_value) const;
	BitBoard clone() const;
	BitBoard& left();
	BitBoard& right();
	BitBoard& up();
	BitBoard& down();
	BitBoard operator|(const BitBoard& that) const;
	BitBoard operator&(const BitBoard& that) const;
	BitBoard operator^(const BitBoard& that) const;
	BitBoard& operator|=(const BitBoard& that);
	BitBoard& operator&=(const BitBoard& that);
	BitBoard& operator^=(const BitBoard& that);
	BitBoard operator~() const;
	bool getSquares(std::vector<Square>& squares) const;
	bool popLowestSquare(Square & square);
	bool getLowestSquare(Square& square) const;
	bool getHighestSquare(Square& square) const;
	std::string toString() const;
private:
	static const uint64_t NOT_A_FILE { ~0x0101010101010101Ui64 };
	static const uint64_t NOT_H_FILE { ~0x8080808080808080Ui64 };
	uint64_t value;
};