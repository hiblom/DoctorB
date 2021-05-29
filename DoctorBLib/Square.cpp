#include "stdafx.h"
#include "Square.h"
#include <cstdint>


//x = a..h = file
//y = 1..8 = row


using namespace std;

Square::Square() {
	value = 0Ui8;
}

Square::Square(uint8_t square_value) {
	value = square_value;
}

Square::Square(uint8_t x, uint8_t y) {
	value = (y << 3) | x;
}

Square::~Square() {
}

uint8_t Square::GetValue() const {
	return value;
}

void Square::SetValue(uint8_t square_value) {
	value = square_value;
}

void Square::SetXy(uint8_t x, uint8_t y) {
	value = (y << 3) | x;
}

inline uint8_t Square::GetX() const {
	return value & MASK_X;
}

inline uint8_t Square::GetY() const {
	return value >> 3;
}

bool Square::operator==(const Square & that) const {
	return value == that.value;
}

string Square::ToString() const {
	char result[3] = { 'a' + GetX(), '1' + GetY(), '\0' };
	return result;
}
