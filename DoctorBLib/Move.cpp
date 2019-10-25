#include "stdafx.h"
#include "Move.h"

Move::Move() {
}

Move::Move(const Square move_square_from, const Square move_square_to) {
	square_from = move_square_from;
	square_to = move_square_to;
}

Move::Move(const Square move_square_from, const Square move_square_to, const Piece move_promo_piece) {
	square_from = move_square_from;
	square_to = move_square_to;
	promo_piece = move_promo_piece;
}

Square Move::GetSquareFrom() const {
	return square_from;
}

Square Move::GetSquareTo() const {
	return square_to;
}

bool Move::GetPromoPiece(Piece& piece) const {
	if (promo_piece.has_value()) {
		piece = promo_piece.value();
		return true;
	}
	
	return false;
}

bool Move::operator==(const Move& that) const {
	return
		square_from == that.square_from &&
		square_to == that.square_to &&
		(!promo_piece.has_value() && !that.promo_piece.has_value() || promo_piece.has_value() && that.promo_piece.has_value() && promo_piece.value() == that.promo_piece.value());
}

std::string Move::ToString() const {
	return square_from.ToString() + square_to.ToString() + (promo_piece.has_value() ? promo_piece.value().ToString() : "");
}

Move::~Move() {
}
