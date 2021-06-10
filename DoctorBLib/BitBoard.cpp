#include "stdafx.h"
#include "BitBoard.h"
#include <intrin.h>

#pragma intrinsic(_BitScanForward64)
#pragma intrinsic(_BitScanReverse64)

using namespace std;

BitBoard::BitBoard() {
	value = 0Ui64;
}

BitBoard::BitBoard(uint64_t bit_board_value) {
	value = bit_board_value;
}

BitBoard::~BitBoard() {
}

bool BitBoard::empty() {
	return value == 0Ui64;
}

bool BitBoard::notEmpty() {
	return value != 0Ui64;
}

uint8_t BitBoard::popCount() const {
	return static_cast<uint8_t>(__popcnt64(value));
}

BitBoard& BitBoard::set(uint8_t square_value) {
	value |= 1Ui64 << square_value;
	return *this;
}

BitBoard& BitBoard::clear(uint8_t square_value) {
	value &= ~(1Ui64 << square_value);
	return *this;
}

bool BitBoard::check(uint8_t square_value) const {
	return (value & (1Ui64 << square_value)) != 0;
}

BitBoard BitBoard::clone() const {
	return BitBoard(value);
}

BitBoard& BitBoard::left() {
	value >>= 1;
	value &= NOT_H_FILE;
	return *this;
}

BitBoard& BitBoard::right() {
	value <<= 1;
	value &= NOT_A_FILE;
	return *this;
}

BitBoard& BitBoard::up() {
	value <<= 8;
	return *this;
}

BitBoard& BitBoard::down() {
	value >>= 8;
	return *this;
}

BitBoard BitBoard::operator|(const BitBoard& that) const {
	return BitBoard(value | that.value);
}

BitBoard BitBoard::operator&(const BitBoard& that) const {
	return BitBoard(value & that.value);
}

BitBoard BitBoard::operator^(const BitBoard& that) const {
	return BitBoard(value ^ that.value);
}

BitBoard& BitBoard::operator|=(const BitBoard& that) {
	value |= that.value;
	return *this;
}

BitBoard& BitBoard::operator&=(const BitBoard& that) {
	value &= that.value;
	return *this;
}

BitBoard & BitBoard::operator^=(const BitBoard& that) {
	value ^= that.value;
	return *this;
}

BitBoard BitBoard::operator~() const {
	return BitBoard(~value);
}

bool BitBoard::getSquares(std::vector<Square>& squares) const {
	uint64_t mask = value;
	unsigned long index;
	while (_BitScanForward64(&index, mask)) {
		squares.push_back(Square(static_cast<uint8_t>(index)));
		mask &= ~(1Ui64 << index);
	}
	return true;
}

bool BitBoard::popLowestSquare(Square& square) {
	if (!value)
		return false;
	
	unsigned long index;
	_BitScanForward64(&index, value);
	square.setValue(static_cast<uint8_t>(index));
	value &= ~(1Ui64 << index);
	return true;
}

bool BitBoard::getLowestSquare(Square& square) const {
	unsigned long index;
	if (_BitScanForward64(&index, value)) {
		square.setValue(static_cast<uint8_t>(index));
		return true;
	}
	return false;
}

bool BitBoard::getHighestSquare(Square& square) const {
	unsigned long index;
	if (_BitScanReverse64(&index, value)) {
		square.setValue(static_cast<uint8_t>(index));
		return true;
	}
	return false;
}

string BitBoard::toString() const {
	string result = "";
	for (uint8_t y = 7; 0 <= y && y <= 7; y--) {
		for (uint8_t x = 0; x <= 7; x++) {
			if (value & (1Ui64 << (y << 3 | x)))
				result += "X";
			else
				result += ".";
		}
		result += "\n";
	}

	return result;
}

