#include "stdafx.h"
#include "Position.h"
#include <array>
#include "Piece.h"
#include "Move.h"
#include "Constants.h"
#include "Zobrist.h"

using namespace std;

Position::Position() {
	//empty board equals hash_key = 0, so no castling status, black to move(!), no pieces
	castling_status_bits_ = 0Ui8;
	active_color_ = Piece::COLOR_BLACK;
	hash_key_ = 0Ui64;
	halfmove_clock_ = 0;
}

Position::~Position() {
}

void Position::setPiece(const Square& square, const Piece& piece) {
	bit_boards_[piece.getValue()].set(square.getValue());
	hash_key_ ^= Zobrist::PIECE_SQUARE_KEY[piece.getValue()][square.getValue()];
}

void Position::clearSquare(const Square& square) {
	for (uint8_t p = 0; p <= 11; p++) {
		if (bit_boards_[p].check(square.getValue())) {
			bit_boards_[p].clear(square.getValue());
			hash_key_ ^= Zobrist::PIECE_SQUARE_KEY[p][square.getValue()];
			break;
		}
	}
}

void Position::clearSquare(const Square& square, const Piece& piece) {
	if (bit_boards_[piece.getValue()].check(square.getValue())) {
		bit_boards_[piece.getValue()].clear(square.getValue());
		hash_key_ ^= Zobrist::PIECE_SQUARE_KEY[piece.getValue()][square.getValue()];
	}
}

bool Position::getPiece(const Square & square, Piece& piece) const {
	for (uint8_t p = 0; p <= 11; p++) {
		if (bit_boards_[p].check(square.getValue())) {
			piece.setValue(p);
			return true;
		}
	}
	return false;
}

int Position::getPieceCount(const Piece& piece) const {
	return bit_boards_[piece.getValue()].popCount();
}

void Position::setActiveColor(uint8_t color) {
	if (color != active_color_) {
		active_color_ = color;
		hash_key_ ^= Zobrist::WHITE_TURN_KEY;
	}
}

void Position::toggleActiveColor() {
	active_color_ ^= 1Ui8;
	hash_key_ ^= Zobrist::WHITE_TURN_KEY;
}

uint8_t Position::getActiveColor() const {
	return active_color_;
}

void Position::setCastlingStatus(int index, bool value) {
	bool current_status = getCastlingStatus(index);
	if (value != current_status) {
		castling_status_bits_ = (castling_status_bits_ & ~(1Ui8 << index)) | (static_cast<uint8_t>(value) << index);
		hash_key_ ^= Zobrist::CASTLING_KEY[index];
	}
}

bool Position::getCastlingStatus(int index) const {
	return castling_status_bits_ & (1Ui8 << index);
}

void Position::setEpSquare(const Square& square) {
	if (ep_square_.has_value()) {
		hash_key_ ^= Zobrist::EP_FILE_KEY[ep_square_.value().getX()];
		ep_square_.reset();
	}

	//polyglot hashing; only set ep square when there is an enemy pawn next to the moved pawn
	bool setEpSquare = false;

	uint8_t other_color = square.getY() == 2Ui8 ? Piece::COLOR_BLACK : Piece::COLOR_WHITE;
	uint8_t other_y = square.getY() == 2Ui8 ? 3Ui8 : 4Ui8;
	Piece other_pawn = Piece(Piece::TYPE_PAWN, other_color);
	if (square.getX() > 0) {
		Piece piece;
		if (getPiece(Square(square.getX() - 1, other_y), piece)) {
			if (piece.getValue() == other_pawn.getValue()) {
				setEpSquare = true;
			}
		}
	}
	if (!setEpSquare && square.getX() < 7) {
		Piece piece;
		if (getPiece(Square(square.getX() + 1, other_y), piece)) {
			if (piece.getValue() == other_pawn.getValue()) {
				setEpSquare = true;
			}
		}
	}

	if (setEpSquare) {
		ep_square_ = square;
		hash_key_ ^= Zobrist::EP_FILE_KEY[square.getX()];
	}
}

void Position::resetEpSquare() {
	if (ep_square_.has_value()) {
		hash_key_ ^= Zobrist::EP_FILE_KEY[ep_square_.value().getX()];
	}
	ep_square_.reset();
}

bool Position::getEpSquare(Square& square) const {
	if (ep_square_.has_value()) {
		square = ep_square_.value();
		return true;
	}

	return false;
}

void Position::setHalfmoveClock(uint16_t value) {
	halfmove_clock_ = value;
}

void Position::incHalfmoveClock() {
	halfmove_clock_++;
}

uint16_t Position::getHalfmoveClock() {
	return halfmove_clock_;
}

string Position::toString() const {
	string result = "";
	Piece piece;
	for (uint8_t y = 7; 0 <= y && y <= 7; y--) {
		result += to_string(y + 1) + ' ';
		for (uint8_t x = 0; x <= 7; x++) {
			if (getPiece(Square(x, y), piece)) {
				result += piece.toChar();
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

bool Position::applyMove(const Move& move) {
	static const array<uint8_t, 2> EP_RANK { { 5Ui8, 2Ui8 } };

	uint8_t inactive_color = active_color_ ^ 1Ui8;

	//get piece from move
	Piece piece;
	move.getPiece(piece);

	//clear square from
	clearSquare(move.getSquareFrom(), piece);

	Piece captured_piece;
	bool is_capture = getPiece(move.getSquareTo(), captured_piece);
	//clear square to (the bitboard containing the captured piece differs from the bitboard containing the moving piece)
	if (is_capture) {
		clearSquare(move.getSquareTo(), captured_piece);
	}

	//if move is promo, piece type will be set to promo piece type
	move.getPromoOrMovingPieceType(piece);

	//set square to
	setPiece(move.getSquareTo(), piece);

	//en-passant -> clear captured pawn, set capture flag
	if (move.isEpCapture()) {
		clearSquare(Square(move.getSquareTo().getX(), move.getSquareFrom().getY()), Piece(Piece::TYPE_PAWN, inactive_color));
		is_capture = true;
	}

	//castling
	if (move.isCastling()) {
		if (move.getSquareTo() == Square(Square::C1)) {
			clearSquare(Square(Square::A1), Piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE));
			setPiece(Square(Square::D1), Piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE));
			setCastlingStatus(Constants::CASTLING_WHITE_KINGSIDE, false);
			setCastlingStatus(Constants::CASTLING_WHITE_QUEENSIDE, false);
		}
		else if (move.getSquareTo() == Square(Square::G1)) {
			clearSquare(Square(Square::H1), Piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE));
			setPiece(Square(Square::F1), Piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE));
			setCastlingStatus(Constants::CASTLING_WHITE_KINGSIDE, false);
			setCastlingStatus(Constants::CASTLING_WHITE_QUEENSIDE, false);
		}
		else if (move.getSquareTo() == Square(Square::C8)) {
			clearSquare(Square(Square::A8), Piece(Piece::TYPE_ROOK, Piece::COLOR_BLACK));
			setPiece(Square(Square::D8), Piece(Piece::TYPE_ROOK, Piece::COLOR_BLACK));
			setCastlingStatus(Constants::CASTLING_BLACK_KINGSIDE, false);
			setCastlingStatus(Constants::CASTLING_BLACK_QUEENSIDE, false);
		}
		else if (move.getSquareTo() == Square(Square::G8)) {
			clearSquare(Square(Square::H8), Piece(Piece::TYPE_ROOK, Piece::COLOR_BLACK));
			setPiece(Square(Square::F8), Piece(Piece::TYPE_ROOK, Piece::COLOR_BLACK));
			setCastlingStatus(Constants::CASTLING_BLACK_KINGSIDE, false);
			setCastlingStatus(Constants::CASTLING_BLACK_QUEENSIDE, false);
		}
	}
	else {
		//reset castling status when king or rook moved or rook captured
		bool castling_status_wq = getCastlingStatus(Constants::CASTLING_WHITE_QUEENSIDE) &&
			!(move.getSquareFrom() == Square(Square::E1) || move.getSquareFrom() == Square(Square::A1) || move.getSquareTo() == Square(Square::A1));
		setCastlingStatus(Constants::CASTLING_WHITE_QUEENSIDE, castling_status_wq);

		bool castling_status_wk = getCastlingStatus(Constants::CASTLING_WHITE_KINGSIDE) &&
			!(move.getSquareFrom() == Square(Square::E1) || move.getSquareFrom() == Square(Square::H1) || move.getSquareTo() == Square(Square::H1));
		setCastlingStatus(Constants::CASTLING_WHITE_KINGSIDE, castling_status_wk);

		bool castling_status_bq =
			getCastlingStatus(Constants::CASTLING_BLACK_QUEENSIDE) &&
			!(move.getSquareFrom() == Square(Square::E8) || move.getSquareFrom() == Square(Square::A8) || move.getSquareTo() == Square(Square::A8));
		setCastlingStatus(Constants::CASTLING_BLACK_QUEENSIDE, castling_status_bq);

		bool castling_status_bk =
			getCastlingStatus(Constants::CASTLING_BLACK_KINGSIDE) &&
			!(move.getSquareFrom() == Square(Square::E8) || move.getSquareFrom() == Square(Square::H8) || move.getSquareTo() == Square(Square::H8));
		setCastlingStatus(Constants::CASTLING_BLACK_KINGSIDE, castling_status_bk);
	}

	if (move.isDoublePush()) {
		setEpSquare(Square(move.getSquareFrom().getX(), EP_RANK[active_color_]));
	}
	else {
		resetEpSquare();
	}

	//increase/reset halfmove clock (reset when pawn moves or capture)
	if (is_capture || move.isPromotion() || piece.getType() == Piece::TYPE_PAWN) {
		setHalfmoveClock(0);
	}
	else {
		incHalfmoveClock();
	}

	toggleActiveColor();

	return true;
}

bool Position::getPieceSquares(const Piece& piece, vector<Square>& squares) const {
	squares.clear();
	return bit_boards_[piece.getValue()].getSquares(squares);
}

bool Position::getPieceSquare(const Piece & piece, Square& square) const {
	return bit_boards_[piece.getValue()].getLowestSquare(square);
}

BitBoard Position::getBitBoard(const Piece& piece) const {
	return bit_boards_[piece.getValue()];
}

//for debugging purposes
void Position::recalculateHashKey() {
	hash_key_ = 0Ui64;
	for (uint8_t p = 0; p < 12; p++) {
		BitBoard b = getBitBoard(p);
		Square s;
		while (b.popLowestSquare(s)) 
			hash_key_ ^= Zobrist::PIECE_SQUARE_KEY[p][s.getValue()];
	}

	if (getActiveColor() == Piece::COLOR_WHITE)
		hash_key_ ^= Zobrist::WHITE_TURN_KEY;

	for (int i = 0; i < 4; i++) {
		if (getCastlingStatus(i))
			hash_key_ ^= Zobrist::CASTLING_KEY[i];
	}

	Square ep_square;
	if (getEpSquare(ep_square))
		hash_key_ ^= Zobrist::EP_FILE_KEY[ep_square.getX()];
}

uint64_t Position::getHashKey() const {
	return hash_key_;
}

uint16_t Position::getHalfmoveClock() const {
	return halfmove_clock_;
}
