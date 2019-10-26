#include "stdafx.h"
#include "Piece.h"
#include "Constants.h"

using namespace std;

Piece::Piece() {
}

Piece::Piece(uint8_t piece_value) {
	value = piece_value;
}

Piece::Piece(uint8_t piece_type, uint8_t piece_color) {
	value = piece_type | piece_color;
}

Piece::~Piece() {
}

uint8_t Piece::GetType() const {
	return value & MASK_TYPE;
}

void Piece::SetType(uint8_t piece_type) {
	value = piece_type | (value & MASK_COLOR);
}

uint8_t Piece::GetColor() const {
	return value & MASK_COLOR;
}

void Piece::SetColor(uint8_t piece_color) {
	value = piece_color & (value & MASK_TYPE);
}

uint8_t Piece::GetValue() const {
	return value;
}

void Piece::SetValue(uint8_t piece_value) {
	value = piece_value;
}

char Piece::ToChar() const {
	if (value > 11)
		return '?';

	return Constants::PIECE_CHARS[value];
}

string Piece::ToString() const {
	char result[2] = { ToChar(), '\0' };
	return result;
}

bool Piece::operator==(const Piece & that) const {
	return value == that.value;
}
