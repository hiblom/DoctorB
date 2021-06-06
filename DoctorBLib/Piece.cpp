#include "stdafx.h"
#include "Piece.h"
#include <array>

#include "Constants.h"

using namespace std;

Piece::Piece() {
	value_ = 0;
}

Piece::Piece(uint8_t piece_value) {
	value_ = piece_value;
}

Piece::Piece(uint8_t piece_type, uint8_t piece_color) {
	value_ = piece_type | piece_color;
}

Piece::~Piece() {
}

uint8_t Piece::getType() const {
	return value_ & MASK_TYPE;
}

void Piece::setType(uint8_t piece_type) {
	value_ = (value_ & ~MASK_TYPE) | piece_type;
}

uint8_t Piece::getColor() const {
	return value_ & MASK_COLOR;
}

void Piece::setColor(uint8_t piece_color) {
	value_ = (value_ & ~MASK_COLOR) | piece_color;
}

uint8_t Piece::getValue() const {
	return value_;
}

void Piece::setValue(uint8_t piece_value) {
	value_ = piece_value;
}

char Piece::toChar() const {
	if (value_ > 11)
		return '?';

	return Constants::PIECE_CHARS[value_];
}

string Piece::toString() const {
	const array<char, 2> result { { toChar(), '\0' } };
	return result.data();
}

bool Piece::operator==(const Piece & that) const {
	return value_ == that.value_;
}
