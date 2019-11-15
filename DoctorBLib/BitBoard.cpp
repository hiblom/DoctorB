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

bool BitBoard::Empty() {
	return value == 0Ui64;
}

bool BitBoard::NotEmpty() {
	return value != 0Ui64;
}

uint8_t BitBoard::PopCount(){
	return (uint8_t)(__popcnt64(value));
}

BitBoard& BitBoard::Set(uint8_t square_value) {
	value |= 1Ui64 << square_value;
	return *this;
}

BitBoard& BitBoard::Clear(uint8_t square_value) {
	value &= ~(1Ui64 << square_value);
	return *this;
}

bool BitBoard::Check(uint8_t square_value) const {
	return (value & (1Ui64 << square_value)) != 0;
}

BitBoard BitBoard::Clone() const {
	return BitBoard(value);
}

BitBoard& BitBoard::Left() {
	value >>= 1;
	value &= NOT_H_FILE;
	return *this;
}

BitBoard& BitBoard::Right() {
	value <<= 1;
	value &= NOT_A_FILE;
	return *this;
}

BitBoard& BitBoard::Up() {
	value <<= 8;
	return *this;
}

BitBoard& BitBoard::Down() {
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

bool BitBoard::GetSquares(std::vector<Square>& squares) const {
	uint64_t mask = value;
	unsigned long index;
	while (_BitScanForward64(&index, mask)) {
		squares.push_back(Square((uint8_t)index));
		mask &= ~(1Ui64 << index);
	}
	return true;
}

bool BitBoard::PopLowestSquare(Square& square) {
	if (!value)
		return false;
	
	unsigned long index;
	_BitScanForward64(&index, value);
	square.SetValue((uint8_t)index);
	value &= ~(1Ui64 << index);
	return true;
}

bool BitBoard::GetLowestSquare(Square& square) const {
	unsigned long index;
	if (_BitScanForward64(&index, value)) {
		square.SetValue((uint8_t)index);
		return true;
	}
	return false;
}

bool BitBoard::GetHighestSquare(Square& square) const {
	unsigned long index;
	if (_BitScanReverse64(&index, value)) {
		square.SetValue((uint8_t)index);
		return true;
	}
	return false;
}

string BitBoard::ToString() const {
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

