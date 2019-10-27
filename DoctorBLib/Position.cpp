#include "stdafx.h"
#include "Position.h"
#include "Piece.h"
#include "Move.h"
#include "Constants.h"

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

void Position::ClearSquare(const Square& square, const Piece& piece) {
	bit_boards[piece.GetValue()].Clear(square.GetValue());
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
	uint8_t inactive_color = active_color ^ 1Ui8;

	//get piece at square from
	Piece piece;
	if (!GetPiece(move.GetSquareFrom(), piece))
		return false;

	//clear square from
	ClearSquare(move.GetSquareFrom(), piece);

	//TODO put capture flag in move (?)
	Piece captured_piece;
	bool is_capture = GetPiece(move.GetSquareTo(), captured_piece);
	//clear square to (the bitboard containing the captured piece is differs from the bitboard containing the moving piece)
	if (is_capture) {
		ClearSquare(move.GetSquareTo(), captured_piece);
	}

	//promo piece?
	Piece promo_piece;
	if (move.GetPromoPiece(promo_piece)) {
		//make piece type the same as promo piece type
		piece.SetType(promo_piece.GetType());
	}

	//set square to
	SetPiece(move.GetSquareTo(), piece);

	//en-passant -> clear captured pawn, set capture flag
	if (move.IsEpCapture()) {
		ClearSquare(Square(move.GetSquareTo().GetX(), move.GetSquareFrom().GetY()), Piece(Piece::TYPE_PAWN, inactive_color));
		is_capture = true;
	}

	//castling
	if (move.IsCastling()) {
		if (move.GetSquareTo() == Square(Square::C1)) {
			ClearSquare(Square(Square::A1), Piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE));
			SetPiece(Square(Square::D1), Piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE));
			SetCastlingStatus(Constants::CASTLING_WHITE_KINGSIDE, false);
			SetCastlingStatus(Constants::CASTLING_WHITE_QUEENSIDE, false);
		}
		else if (move.GetSquareTo() == Square(Square::G1)) {
			ClearSquare(Square(Square::H1), Piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE));
			SetPiece(Square(Square::F1), Piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE));
			SetCastlingStatus(Constants::CASTLING_WHITE_KINGSIDE, false);
			SetCastlingStatus(Constants::CASTLING_WHITE_QUEENSIDE, false);
		}
		else if (move.GetSquareTo() == Square(Square::C8)) {
			ClearSquare(Square(Square::A8), Piece(Piece::TYPE_ROOK, Piece::COLOR_BLACK));
			SetPiece(Square(Square::D8), Piece(Piece::TYPE_ROOK, Piece::COLOR_BLACK));
			SetCastlingStatus(Constants::CASTLING_BLACK_KINGSIDE, false);
			SetCastlingStatus(Constants::CASTLING_BLACK_QUEENSIDE, false);
		}
		else if (move.GetSquareTo() == Square(Square::G8)) {
			ClearSquare(Square(Square::H8), Piece(Piece::TYPE_ROOK, Piece::COLOR_BLACK));
			SetPiece(Square(Square::F8), Piece(Piece::TYPE_ROOK, Piece::COLOR_BLACK));
			SetCastlingStatus(Constants::CASTLING_BLACK_KINGSIDE, false);
			SetCastlingStatus(Constants::CASTLING_BLACK_QUEENSIDE, false);
		}
	}
	else {
		//reset castling status when king or rook moved or rook captured
		if (GetCastlingStatus(Constants::CASTLING_WHITE_QUEENSIDE) && 
			(move.GetSquareFrom() == Square(Square::E1) || move.GetSquareFrom() == Square(Square::A1) || move.GetSquareTo() == Square(Square::A1)))
			SetCastlingStatus(Constants::CASTLING_WHITE_QUEENSIDE, false);
		
		if (GetCastlingStatus(Constants::CASTLING_WHITE_KINGSIDE) && 
			(move.GetSquareFrom() == Square(Square::E1) || move.GetSquareFrom() == Square(Square::H1) || move.GetSquareTo() == Square(Square::H1)))
			SetCastlingStatus(Constants::CASTLING_WHITE_KINGSIDE, false);
		
		if (GetCastlingStatus(Constants::CASTLING_BLACK_QUEENSIDE) && 
			(move.GetSquareFrom() == Square(Square::E8) || move.GetSquareFrom() == Square(Square::A8) || move.GetSquareTo() == Square(Square::A8)))
			SetCastlingStatus(Constants::CASTLING_BLACK_QUEENSIDE, false);
		
		if (GetCastlingStatus(Constants::CASTLING_BLACK_KINGSIDE) && 
			(move.GetSquareFrom() == Square(Square::E8) || move.GetSquareFrom() == Square(Square::H8) || move.GetSquareTo() == Square(Square::H8)))
			SetCastlingStatus(Constants::CASTLING_BLACK_KINGSIDE, false);
	}

	if (move.IsDoublePush()) {
		if (active_color == Piece::COLOR_WHITE)
			ep_square = Square(move.GetSquareFrom().GetX(), 2Ui8);
		else
			ep_square = Square(move.GetSquareFrom().GetX(), 5Ui8);
	}
	else {
		ep_square.reset();
	}

	//TODO hash codes

	//increase/reset halfmove clock (reset when pawn moves or capture)
	if (is_capture || move.IsPromotion() || piece.GetType() == Piece::TYPE_PAWN)
		halfmove_clock = 0;
	else
		halfmove_clock++;

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
