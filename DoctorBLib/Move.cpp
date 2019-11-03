#include "stdafx.h"
#include "Move.h"

Move::Move() {
}

Move::Move(const Piece piece, const Square square_from, const Square square_to) {
	square_from_ = square_from;
	square_to_ = square_to;
	status_bits_ = 0Ui8;
	piece_ = piece;
	promo_piece_ = piece; //set promo piece to moving piece when no promo
}

Move::Move(const Piece piece, const Square square_from, const Square square_to, const Piece promo_piece) {
	square_from_ = square_from;
	square_to_ = square_to;
	status_bits_ = MASK_PROMOTION;
	piece_ = piece;
	promo_piece_ = promo_piece;
}


Move::Move(const Square square_from, const Square square_to) {
	square_from_ = square_from;
	square_to_ = square_to;
	status_bits_ = 0Ui8;
	piece_ = Piece(0Ui8);
	promo_piece_ = Piece(0Ui8);
}

Move::Move(const Square square_from, const Square square_to, const Piece promo_piece) {
	square_from_ = square_from;
	square_to_ = square_to;
	status_bits_ = MASK_PROMOTION;
	piece_ = Piece(0Ui8);
	promo_piece_ = promo_piece;
}

Move::~Move() {
}

Square Move::GetSquareFrom() const {
	return square_from_;
}

Square Move::GetSquareTo() const {
	return square_to_;
}

bool Move::GetPromoPiece(Piece& promo_piece) const {
	if(status_bits_ & MASK_PROMOTION) {
		promo_piece.SetValue(promo_piece_.GetValue());
		return true;
	}
	return false;
}

void Move::GetPromoOrMovingPieceType(Piece& piece) const {
	piece.SetType(promo_piece_.GetType());
}

bool Move::operator==(const Move& that) const {
	return
		square_from_ == that.square_from_ &&
		square_to_ == that.square_to_ &&
		status_bits_ == that.status_bits_ &&
		piece_ == that.piece_ &&
		promo_piece_ == that.promo_piece_;
}

bool Move::IsPromotion() const {
	return status_bits_ & MASK_PROMOTION;
}

bool Move::IsEpCapture() const {
	return status_bits_ & MASK_EP_CAPTURE;
}

Move& Move::SetEpCapture() {
	status_bits_ |= MASK_EP_CAPTURE;
	return *this;
}

bool Move::IsCastling() const {
	return status_bits_ & MASK_CASTLING;
}

Move& Move::SetCastling() {
	status_bits_ |= MASK_CASTLING;
	return *this;
}

bool Move::IsDoublePush() const {
	return status_bits_ & MASK_DOUBLE_PUSH;
}

Move& Move::SetDoublePush() {
	status_bits_ |= MASK_DOUBLE_PUSH;
	return *this;
}

std::string Move::ToString() const {
	if (square_from_.GetValue() == 0 && square_to_.GetValue() == 0)
		return "-";

	if (status_bits_ & MASK_PROMOTION) {
		return square_from_.ToString() + square_to_.ToString() + promo_piece_.ToString();
	}

	return square_from_.ToString() + square_to_.ToString();
}

Move& Move::SetPiece(const Piece piece) {
	piece_ = piece;
	if (!IsPromotion())
		promo_piece_ = piece; //set promo piece to moving piece when no promo
	return *this;
}

void Move::GetPiece(Piece& piece) const {
	piece.SetValue(piece_.GetValue());
}

