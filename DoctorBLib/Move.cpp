#include "stdafx.h"
#include "Move.h"

Move::Move() {
}

Move::Move(const Square move_square_from, const Square move_square_to) {
	square_from = move_square_from;
	square_to = move_square_to;
	status_bits = 0Ui8;
	promo_piece = Piece(0Ui8);
	piece = Piece(0Ui8);
}

Move::Move(const Square move_square_from, const Square move_square_to, const Piece move_promo_piece) {
	square_from = move_square_from;
	square_to = move_square_to;
	status_bits = MASK_PROMOTION;
	promo_piece = move_promo_piece;
	piece = Piece(0Ui8);
}

Square Move::GetSquareFrom() const {
	return square_from;
}

Square Move::GetSquareTo() const {
	return square_to;
}

bool Move::GetPromoPiece(Piece& move_promo_piece) const {
	if (status_bits & MASK_PROMOTION) {
		move_promo_piece.SetValue(promo_piece.GetValue());
		return true;
	}
	return false;
}

bool Move::operator==(const Move& that) const {
	return
		square_from == that.square_from &&
		square_to == that.square_to &&
		status_bits == that.status_bits &&
		promo_piece == that.promo_piece;
}

//inline bool Move::IsCapture() const {
//	return status & MASK_CAPTURE;
//}
//
//Move& Move::SetCapture() {
//	status |= MASK_CAPTURE;
//	return *this;
//}

bool Move::IsPromotion() const {
	return status_bits & MASK_PROMOTION;
}

bool Move::IsEpCapture() const {
	return status_bits & MASK_EP_CAPTURE;
}

Move& Move::SetEpCapture() {
	status_bits |= MASK_EP_CAPTURE;
	return *this;
}

bool Move::IsCastling() const {
	return status_bits & MASK_CASTLING;
}

Move& Move::SetCastling() {
	status_bits |= MASK_CASTLING;
	return *this;
}

bool Move::IsDoublePush() const {
	return status_bits & MASK_DOUBLE_PUSH;
}

Move& Move::SetDoublePush() {
	status_bits |= MASK_DOUBLE_PUSH;
	return *this;
}

std::string Move::ToString() const {
	if (status_bits & MASK_PROMOTION) {
		square_from.ToString() + square_to.ToString() + promo_piece.ToString();
	}

	return square_from.ToString() + square_to.ToString();
}

Move& Move::SetPiece(const Piece move_piece) {
	piece = move_piece;
	return *this;
}

void Move::GetPiece(Piece& move_piece) const {
	move_piece.SetValue(piece.GetValue());
}

Move::~Move() {
}
