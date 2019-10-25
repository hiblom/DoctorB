#include "stdafx.h"
#include "Piece.h"

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
	switch (value)
	{
	case TYPE_PAWN | COLOR_WHITE:
		return 'P';
	case TYPE_PAWN | COLOR_BLACK:
		return 'p';
	case TYPE_KING | COLOR_WHITE:
		return 'K';
	case TYPE_KING | COLOR_BLACK:
		return 'k';
	case TYPE_QUEEN | COLOR_WHITE:
		return 'Q';
	case TYPE_QUEEN | COLOR_BLACK:
		return 'q';
	case TYPE_ROOK| COLOR_WHITE:
		return 'R';
	case TYPE_ROOK | COLOR_BLACK:
		return 'r';
	case TYPE_BISHOP | COLOR_WHITE:
		return 'B';
	case TYPE_BISHOP | COLOR_BLACK:
		return 'b';
	case TYPE_KNIGHT | COLOR_WHITE:
		return 'N';
	case TYPE_KNIGHT | COLOR_BLACK:
		return 'n';
	default:
		return '?';
	}
}

string Piece::ToString() const {
	char result[2] = { ToChar(), '\0' };
	return result;
}

bool Piece::operator==(const Piece & that) const {
	return value == that.value;
}
