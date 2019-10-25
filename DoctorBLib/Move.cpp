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
	status = MASK_PROMOTION | (move_promo_piece.GetValue() << 8);
}

Square Move::GetSquareFrom() const {
	return square_from;
}

Square Move::GetSquareTo() const {
	return square_to;
}

bool Move::GetPromoPiece(Piece& piece) const {
	if (status & MASK_PROMOTION) {
		piece.SetValue((status & MASK_PROMO_PIECE) >> 8);
		return true;
	}
	return false;
}

bool Move::operator==(const Move& that) const {
	return
		square_from == that.square_from &&
		square_to == that.square_to &&
		status == that.status;
}

//inline bool Move::IsCapture() const {
//	return status & MASK_CAPTURE;
//}
//
//Move& Move::SetCapture() {
//	status |= MASK_CAPTURE;
//	return *this;
//}

inline bool Move::IsPromotion() const {
	return status & MASK_PROMOTION;
}

inline bool Move::IsEpCapture() const {
	return status & MASK_EP_CAPTURE;
}

Move& Move::SetEpCapture() {
	status |= MASK_EP_CAPTURE;
	return *this;
}

inline bool Move::IsCastling() const {
	return status & MASK_CASTLING;
}

Move& Move::SetCastling() {
	status |= MASK_CASTLING;
	return *this;
}

bool Move::IsDoublePush() const {
	return status & MASK_DOUBLE_PUSH;
}

Move& Move::SetDoublePush() {
	status |= MASK_DOUBLE_PUSH;
	return *this;
}

std::string Move::ToString() const {
	Piece promo_piece;
	if (GetPromoPiece(promo_piece)) {
		square_from.ToString() + square_to.ToString() + promo_piece.ToString();
	}

	return square_from.ToString() + square_to.ToString();
}

Move::~Move() {
}
