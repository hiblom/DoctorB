#include "stdafx.h"
#include "Move.h"

using namespace std;

Move::Move() {
	status_bits_ = 0Ui8;
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

Square Move::getSquareFrom() const {
	return square_from_;
}

Square Move::getSquareTo() const {
	return square_to_;
}

bool Move::getPromoPiece(Piece& promo_piece) const {
	if(status_bits_ & MASK_PROMOTION) {
		promo_piece.setValue(promo_piece_.getValue());
		return true;
	}
	return false;
}

void Move::getPromoOrMovingPieceType(Piece& piece) const {
	piece.setType(promo_piece_.getType());
}

bool Move::operator==(const Move& that) const {
	return
		square_from_ == that.square_from_ &&
		square_to_ == that.square_to_ &&
		status_bits_ == that.status_bits_ &&
		piece_ == that.piece_ &&
		promo_piece_ == that.promo_piece_;
}

bool Move::isPromotion() const {
	return status_bits_ & MASK_PROMOTION;
}

bool Move::isEpCapture() const {
	return status_bits_ & MASK_EP_CAPTURE;
}

Move& Move::setEpCapture() {
	status_bits_ |= MASK_EP_CAPTURE;
	status_bits_ |= MASK_CAPTURE;
	return *this;
}

bool Move::isCapture() const {
	return status_bits_ & MASK_CAPTURE;
}

Move & Move::setCapture(bool capture) {
	status_bits_ = (status_bits_ & ~MASK_CAPTURE) | (capture * MASK_CAPTURE);
	return *this;
}

bool Move::isCastling() const {
	return status_bits_ & MASK_CASTLING;
}

Move& Move::setCastling() {
	status_bits_ |= MASK_CASTLING;
	return *this;
}

bool Move::isDoublePush() const {
	return status_bits_ & MASK_DOUBLE_PUSH;
}

Move& Move::setDoublePush() {
	status_bits_ |= MASK_DOUBLE_PUSH;
	return *this;
}

std::string Move::toString() const {
	if (!isValid())
		return "";

	if (isPromotion()) {
		string promo_letter = string(1, tolower(promo_piece_.toChar()));
		return square_from_.toString() + square_to_.toString() + promo_letter;
	}

	return square_from_.toString() + square_to_.toString();
}

Move& Move::setPiece(const Piece piece) {
	piece_ = piece;
	if (!isPromotion())
		promo_piece_ = piece; //set promo piece to moving piece when no promo
	return *this;
}

void Move::getPiece(Piece& piece) const {
	piece.setValue(piece_.getValue());
}

bool Move::isValid() const {
	return square_from_.getValue() != 0 || square_to_.getValue() != 0;
}

