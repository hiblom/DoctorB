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
	castling_status_bits = 0Ui8;
	active_color = Piece::COLOR_BLACK;
	hash_key = 0Ui64;
	halfmove_clock = 0;
}

Position::~Position() {
}

void Position::SetPiece(const Square& square, const Piece& piece) {
	bit_boards[piece.GetValue()].Set(square.GetValue());
	hash_key ^= Zobrist::PIECE_SQUARE_KEY[piece.GetValue()][square.GetValue()];
}

void Position::ClearSquare(const Square& square) {
	for (uint8_t p = 0; p <= 11; p++) {
		if (bit_boards[p].Check(square.GetValue())) {
			bit_boards[p].Clear(square.GetValue());
			hash_key ^= Zobrist::PIECE_SQUARE_KEY[p][square.GetValue()];
			break;
		}
	}
}

void Position::ClearSquare(const Square& square, const Piece& piece) {
	if (bit_boards[piece.GetValue()].Check(square.GetValue())) {
		bit_boards[piece.GetValue()].Clear(square.GetValue());
		hash_key ^= Zobrist::PIECE_SQUARE_KEY[piece.GetValue()][square.GetValue()];
	}
}

bool Position::GetPiece(const Square & square, Piece& piece) const {
	for (uint8_t p = 0; p <= 11; p++) {
		if (bit_boards[p].Check(square.GetValue())) {
			piece.SetValue(p);
			return true;
		}
	}
	return false;
}

int Position::GetPieceCount(const Piece& piece) const {
	return bit_boards[piece.GetValue()].PopCount();
}

void Position::SetActiveColor(uint8_t color) {
	if (color != active_color) {
		active_color = color;
		hash_key ^= Zobrist::WHITE_TURN_KEY;
	}
}

void Position::ToggleActiveColor() {
	active_color ^= 1Ui8;
	hash_key ^= Zobrist::WHITE_TURN_KEY;
}

uint8_t Position::GetActiveColor() const {
	return active_color;
}

void Position::SetCastlingStatus(int index, bool value) {
	bool current_status = GetCastlingStatus(index);
	if (value != current_status) {
		castling_status_bits = (castling_status_bits & ~(1Ui8 << index)) | (static_cast<uint8_t>(value) << index);
		hash_key ^= Zobrist::CASTLING_KEY[index];
	}
}

bool Position::GetCastlingStatus(int index) const {
	return castling_status_bits & (1Ui8 << index);
}

void Position::SetEpSquare(const Square& square) {
	if (ep_square.has_value()) {
		hash_key ^= Zobrist::EP_FILE_KEY[ep_square.value().GetX()];
	}

	//polyglot hashing; only set ep square when there is an enemy pawn next to the moved pawn
	bool setEpSquare = false;

	uint8_t other_color = square.GetY() == 2Ui8 ? Piece::COLOR_BLACK : Piece::COLOR_WHITE;
	uint8_t other_y = square.GetY() == 2Ui8 ? 3Ui8 : 4Ui8;
	Piece other_pawn = Piece(Piece::TYPE_PAWN, other_color);
	if (square.GetX() > 0) {
		Piece piece;
		if (GetPiece(Square(square.GetX() - 1, other_y), piece)) {
			if (piece.GetValue() == other_pawn.GetValue()) {
				setEpSquare = true;
			}
		}
	}
	if (!setEpSquare && square.GetX() < 7) {
		Piece piece;
		if (GetPiece(Square(square.GetX() + 1, other_y), piece)) {
			if (piece.GetValue() == other_pawn.GetValue()) {
				setEpSquare = true;
			}
		}
	}

	if (setEpSquare) {
		ep_square = square;
		hash_key ^= Zobrist::EP_FILE_KEY[square.GetX()];
	}
}

void Position::ResetEpSquare() {
	if (ep_square.has_value()) {
		hash_key ^= Zobrist::EP_FILE_KEY[ep_square.value().GetX()];
	}
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

void Position::IncHalfmoveClock() {
	halfmove_clock++;
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
	static const array<uint8_t, 2> EP_RANK { { 5Ui8, 2Ui8 } };

	uint8_t inactive_color = active_color ^ 1Ui8;

	//get piece from move
	Piece piece;
	move.GetPiece(piece);

	//clear square from
	ClearSquare(move.GetSquareFrom(), piece);

	Piece captured_piece;
	bool is_capture = GetPiece(move.GetSquareTo(), captured_piece);
	//clear square to (the bitboard containing the captured piece differs from the bitboard containing the moving piece)
	if (is_capture) {
		ClearSquare(move.GetSquareTo(), captured_piece);
	}

	//if move is promo, piece type will be set to promo piece type
	move.GetPromoOrMovingPieceType(piece);

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
		bool castling_status_wq = GetCastlingStatus(Constants::CASTLING_WHITE_QUEENSIDE) &&
			!(move.GetSquareFrom() == Square(Square::E1) || move.GetSquareFrom() == Square(Square::A1) || move.GetSquareTo() == Square(Square::A1));
		SetCastlingStatus(Constants::CASTLING_WHITE_QUEENSIDE, castling_status_wq);

		bool castling_status_wk = GetCastlingStatus(Constants::CASTLING_WHITE_KINGSIDE) &&
			!(move.GetSquareFrom() == Square(Square::E1) || move.GetSquareFrom() == Square(Square::H1) || move.GetSquareTo() == Square(Square::H1));
		SetCastlingStatus(Constants::CASTLING_WHITE_KINGSIDE, castling_status_wk);

		bool castling_status_bq =
			GetCastlingStatus(Constants::CASTLING_BLACK_QUEENSIDE) &&
			!(move.GetSquareFrom() == Square(Square::E8) || move.GetSquareFrom() == Square(Square::A8) || move.GetSquareTo() == Square(Square::A8));
		SetCastlingStatus(Constants::CASTLING_BLACK_QUEENSIDE, castling_status_bq);

		bool castling_status_bk =
			GetCastlingStatus(Constants::CASTLING_BLACK_KINGSIDE) &&
			!(move.GetSquareFrom() == Square(Square::E8) || move.GetSquareFrom() == Square(Square::H8) || move.GetSquareTo() == Square(Square::H8));
		SetCastlingStatus(Constants::CASTLING_BLACK_KINGSIDE, castling_status_bk);
	}

	if (move.IsDoublePush())
		SetEpSquare(Square(move.GetSquareFrom().GetX(), EP_RANK[active_color]));
	else
		ResetEpSquare();

	//increase/reset halfmove clock (reset when pawn moves or capture)
	if (is_capture || move.IsPromotion() || piece.GetType() == Piece::TYPE_PAWN)
		SetHalfmoveClock(0);
	else
		IncHalfmoveClock();

	ToggleActiveColor();

	return true;
}

bool Position::GetPieceSquares(const Piece& piece, vector<Square>& squares) const {
	squares.clear();
	return bit_boards[piece.GetValue()].GetSquares(squares);
}

bool Position::GetPieceSquare(const Piece & piece, Square& square) const {
	return bit_boards[piece.GetValue()].GetLowestSquare(square);
}

BitBoard Position::GetBitBoard(const Piece& piece) const {
	return bit_boards[piece.GetValue()];
}

//for debugging purposes
void Position::RecalculateHashKey() {
	hash_key = 0Ui64;
	for (uint8_t p = 0; p < 12; p++) {
		BitBoard b = GetBitBoard(p);
		Square s;
		while (b.PopLowestSquare(s)) 
			hash_key ^= Zobrist::PIECE_SQUARE_KEY[p][s.GetValue()];
	}

	if (GetActiveColor() == Piece::COLOR_WHITE)
		hash_key ^= Zobrist::WHITE_TURN_KEY;

	for (int i = 0; i < 4; i++) {
		if (GetCastlingStatus(i))
			hash_key ^= Zobrist::CASTLING_KEY[i];
	}

	Square ep_square;
	if (GetEpSquare(ep_square))
		hash_key ^= Zobrist::EP_FILE_KEY[ep_square.GetX()];
}

uint64_t Position::GetHashKey() const {
	return hash_key;
}
