#include "stdafx.h"
#include "MoveBoard.h"
#include "BitBoard.h"
#include "Piece.h"
#include "Constants.h"

MoveBoard::MoveBoard() {
}

void MoveBoard::InitializeKingMoves() {
	for (uint8_t s = 0; s < 64; s++) {
		king_moves[s].Set(s);
		king_moves[s] |= king_moves[s].Clone().Left() | king_moves[s].Clone().Right();
		king_moves[s] |= king_moves[s].Clone().Up() | king_moves[s].Clone().Down();
		king_moves[s].Clear(s);
	}
}

void MoveBoard::InitializeKnightMoves() {
	for (uint8_t s = 0; s < 64; s++) {
		BitBoard bb_vert;
		bb_vert.Set(s);
		bb_vert = bb_vert.Clone().Left() | bb_vert.Clone().Right();
		bb_vert = bb_vert.Clone().Up().Up() | bb_vert.Clone().Down().Down();

		BitBoard bb_hor;
		bb_hor.Set(s);
		bb_hor = bb_hor.Clone().Up() | bb_hor.Clone().Down();
		bb_hor = bb_hor.Clone().Left().Left() | bb_hor.Clone().Right().Right();

		knight_moves[s] = bb_vert | bb_hor;
	}
}

void MoveBoard::InitializePawnPushes() {
	for (uint8_t s = 0; s < 64; s++) {
		//double push will be handled in move generator
		pawn_pushes[Piece::COLOR_WHITE][s].Set(s).Up();
		pawn_pushes[Piece::COLOR_BLACK][s].Set(s).Down();
	}
}

void MoveBoard::InitializePawnCaptures() {
	for (uint8_t s = 0; s < 64; s++) {
		BitBoard white_board;
		white_board.Set(s);
		pawn_captures[Piece::COLOR_WHITE][s] = white_board.Clone().Up().Left() | white_board.Clone().Up().Right();

		BitBoard black_board;
		black_board.Set(s);
		pawn_captures[Piece::COLOR_BLACK][s] = black_board.Clone().Down().Left() | black_board.Clone().Down().Right();
	}
}

void MoveBoard::InitializeRays() {
	InitializeRayUp();
	InitializeRayUpRight();
	InitializeRayRight();
	InitializeRayDownRight();
	InitializeRayDown();
	InitializeRayDownLeft();
	InitializeRayLeft();
	InitializeRayUpLeft();
}

void MoveBoard::InitializeRayUp() {
	for (uint8_t s = 0; s < 64; s++) {
		BitBoard board = BitBoard().Set(s);
		while (true) {
			board.Up();
			if (board.Empty())
				break;
			rays[DIR_UP][s] |= board;
		}
	}
}

void MoveBoard::InitializeRayUpRight() {
	for (uint8_t s = 0; s < 64; s++) {
		BitBoard board = BitBoard().Set(s);
		while (true) {
			board.Up().Right();
			if (board.Empty())
				break;
			rays[DIR_UP_RIGHT][s] |= board;
		}
	}
}

void MoveBoard::InitializeRayRight() {
	for (uint8_t s = 0; s < 64; s++) {
		BitBoard board = BitBoard().Set(s);
		while (true) {
			board.Right();
			if (board.Empty())
				break;
			rays[DIR_RIGHT][s] |= board;
		}
	}
}

void MoveBoard::InitializeRayDownRight() {
	for (uint8_t s = 0; s < 64; s++) {
		BitBoard board = BitBoard().Set(s);
		while (true) {
			board.Down().Right();
			if (board.Empty())
				break;
			rays[DIR_DOWN_RIGHT][s] |= board;
		}
	}
}

void MoveBoard::InitializeRayDown() {
	for (uint8_t s = 0; s < 64; s++) {
		BitBoard board = BitBoard().Set(s);
		while (true) {
			board.Down();
			if (board.Empty())
				break;
			rays[DIR_DOWN][s] |= board;
		}
	}
}

void MoveBoard::InitializeRayDownLeft() {
	for (uint8_t s = 0; s < 64; s++) {
		BitBoard board = BitBoard().Set(s);
		while (true) {
			board.Down().Left();
			if (board.Empty())
				break;
			rays[DIR_DOWN_LEFT][s] |= board;
		}
	}
}

void MoveBoard::InitializeRayLeft() {
	for (uint8_t s = 0; s < 64; s++) {
		BitBoard board = BitBoard().Set(s);
		while (true) {
			board.Left();
			if (board.Empty())
				break;
			rays[DIR_LEFT][s] |= board;
		}
	}
}

void MoveBoard::InitializeRayUpLeft() {
	for (uint8_t s = 0; s < 64; s++) {
		BitBoard board = BitBoard().Set(s);
		while (true) {
			board.Up().Left();
			if (board.Empty())
				break;
			rays[DIR_UP_LEFT][s] |= board;
		}
	}
}

void MoveBoard::InitializeCastlingSquares() {
	castling_safe_squares[Constants::CASTLING_WHITE_KINGSIDE] =   0b00110000Ui64;
	castling_empty_squares[Constants::CASTLING_WHITE_KINGSIDE] =  0b01100000Ui64;
	
	castling_safe_squares[Constants::CASTLING_WHITE_QUEENSIDE] =  0b00011000Ui64;
	castling_empty_squares[Constants::CASTLING_WHITE_QUEENSIDE] = 0b00001110Ui64;

	castling_safe_squares[Constants::CASTLING_BLACK_KINGSIDE] =   0b00110000Ui64 << 56;
	castling_empty_squares[Constants::CASTLING_BLACK_KINGSIDE] =  0b01100000Ui64 << 56;
	
	castling_safe_squares[Constants::CASTLING_BLACK_QUEENSIDE] =  0b00011000Ui64 << 56;
	castling_empty_squares[Constants::CASTLING_BLACK_QUEENSIDE] = 0b00001110Ui64 << 56;
}

MoveBoard& MoveBoard::GetInstance() {
	static MoveBoard instance;
	return instance;
}

void MoveBoard::Initialize() {
	InitializeKingMoves();
	InitializeKnightMoves();
	InitializePawnPushes();
	InitializePawnCaptures();
	InitializeRays();
	InitializeCastlingSquares();
}

BitBoard MoveBoard::GetKingMoves(const Square square) const {
	return king_moves[square.GetValue()];
}

BitBoard MoveBoard::GetKnightMoves(const Square square) const {
	return knight_moves[square.GetValue()];
}

BitBoard MoveBoard::GetPawnPushes(const Square square, uint8_t color) const {
	return pawn_pushes[color][square.GetValue()];
}

BitBoard MoveBoard::GetPawnCaptures(const Square square, uint8_t color) const {
	return pawn_captures[color][square.GetValue()];
}

BitBoard MoveBoard::GetRay(const Square square, const int dir) const {
	return rays[dir][square.GetValue()];
}

BitBoard MoveBoard::GetCastlingEmptySquares(const int index) const {
	return castling_empty_squares[index];
}

BitBoard MoveBoard::GetCastlingSafeSquares(const int index) const {
	return castling_safe_squares[index];
}

MoveBoard::~MoveBoard() {
}
