#pragma once
#include <cstdint>
#include <vector>
#include "Square.h"

class BitBoard
{
public:
	BitBoard();
	BitBoard(uint64_t bit_board_value);
	~BitBoard();
	bool IsEmpty();
	void Set(uint8_t square_value);
	void Clear(uint8_t square_value);
	bool Check(uint8_t square_value) const;
	BitBoard Clone() const;
	BitBoard& Left();
	BitBoard& Right();
	BitBoard& Up();
	BitBoard& Down();
	BitBoard operator|(const BitBoard& that) const;
	BitBoard operator&(const BitBoard& that) const;
	BitBoard& operator|=(const BitBoard& that);
	BitBoard& operator&=(const BitBoard& that);
	BitBoard operator~() const;
	bool GetSquares(std::vector<Square>& squares) const;
	bool GetLowestSquare(Square& square) const;
	bool GetHighestSquare(Square& square) const;
private:
	static const uint64_t A_FILE = 0x0101010101010101;
	static const uint64_t H_FILE = 0x8080808080808080;
	uint64_t value;
};