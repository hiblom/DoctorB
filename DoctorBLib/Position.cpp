#include "stdafx.h"
#include "Position.h"
#include "Piece.h"

using namespace std;

Position::Position() {
}

Position::~Position() {
}

void Position::SetPiece(const Square& square, const Piece& piece) {
	bit_boards[piece.GetValue()].Set(square.GetValue());
}

void Position::ClearSquare(const Square& square) {
	for (uint8_t i = 0; i <= 11; i++) {
		bit_boards[i].Clear(square.GetValue());
	}
}

bool Position::GetPiece(const Square & square, Piece& piece) const {
	for (uint8_t i = 0; i <= 11; i++) {
		if (bit_boards[i].Check(square.GetValue())) {
			piece.SetValue(i);
			return true;
		}
	}
	return false;
}

void Position::SetActiveColor(uint8_t color) {
	active_color = color;
}

uint8_t Position::GetActiveColor() const {
	return active_color;
}

void Position::SetCastlingStatus(int index, bool value) {
	castling_status[index] = value;
}

bool Position::GetCastlingStatus(int index) const {
	return castling_status[index];
}

void Position::SetEpSquare(const Square& square) {
	ep_square = square;
}

void Position::ResetEpSquare() {
	ep_square.reset();
}

bool Position::GetEpSquare(Square& square) const {
	if (ep_square.has_value()) {
		square = ep_square.value();
		return true;
	}

	return false;
}

void Position::SetHalfmoveClock(uint16_t value) {
	halfmove_clock = value;
}

uint16_t Position::GetHalfmoveClock() {
	return halfmove_clock;
}

void Position::SetFullmoveNumber(uint16_t value) {
	fullmove_number = value;
}

uint16_t Position::GetFullmoveNumber() {
	return fullmove_number;
}

string Position::ToString() const {
	string result = "";
	Piece piece;
	for (uint8_t y = 7; 0 <= y && y <= 7; y--) {
		result += to_string(y + 1) + ' ';
		for (uint8_t x = 0; x <= 7; x++) {
			if (GetPiece(Square(x, y), piece)) {
				result += piece.ToChar();
			}
			else {
				result += '.';
			}
		}
		result += '\n';
	}
	result += "  abcdefgh\n";
	return result;
}

bool Position::ApplyMove(const Move& move) {
	//get piece at square from
	Piece piece;
	if (!GetPiece(move.GetSquareFrom(), piece))
		return false;

	//clear square from
	ClearSquare(move.GetSquareFrom());

	Piece captured_piece;
	bool is_capture = GetPiece(move.GetSquareTo(), captured_piece);
	//clear square to (the bitboard containing the captured piece is differs from the bitboard containing the moving piece)
	if (is_capture) {
		ClearSquare(move.GetSquareTo());
	}

	//promo piece?
	Piece promo_piece;
	if (move.GetPromoPiece(promo_piece)) {
		//make piece type the same as promo piece type
		piece.SetType(promo_piece.GetType());
	}

	//set square to
	SetPiece(move.GetSquareTo(), piece);

	//TODO en-passant -> clear square behind pawn, also set capture flag

	//TODO castling
	//TODO castling status
	//TODO clear/setting EP square
	//TODO hash codes

	//increase/reset halfmove clock (reset when pawn moves or capture)
	if (is_capture || piece.GetType() == Piece::TYPE_PAWN)
		halfmove_clock = 0;
	else
		halfmove_clock++;

	//increase fullmove number (when black)
	fullmove_number += (active_color & 1Ui8);

	//toggle active color
	active_color ^= 1Ui8;

	return true;
}

bool Position::GetPieceSquares(const Piece& piece, vector<Square>& squares) const {
	return bit_boards[piece.GetValue()].GetSquares(squares);
}

BitBoard Position::GetBitBoard(const Piece& piece) const {
	return bit_boards[piece.GetValue()];
}
