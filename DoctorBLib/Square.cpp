#include "stdafx.h"
#include "Square.h"
#include <cstdint>


//x = a..h = file
//y = 1..8 = row


using namespace std;

Square::Square() {
	value_ = 0Ui8;
}

Square::Square(uint8_t square_value) {
	value_ = square_value;
}

Square::Square(uint8_t x, uint8_t y) {
	value_ = (y << 3) | x;
}

Square::~Square() {
}

uint8_t Square::getValue() const {
	return value_;
}

void Square::setValue(uint8_t square_value) {
	value_ = square_value;
}

void Square::setXy(uint8_t x, uint8_t y) {
	value_ = (y << 3) | x;
}

inline uint8_t Square::getX() const {
	return value_ & MASK_X;
}

inline uint8_t Square::getY() const {
	return value_ >> 3;
}

bool Square::operator==(const Square & that) const {
	return value_ == that.value_;
}

string Square::toString() const {
	char result[3] = { 'a' + getX(), '1' + getY(), '\0' };
	return result;
}
