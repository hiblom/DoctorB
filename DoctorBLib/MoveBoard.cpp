#include "stdafx.h"
#include "MoveBoard.h"
#include "BitBoard.h"
#include "Piece.h"
#include "Constants.h"

MoveBoard::MoveBoard() {
	initialize();
}

void MoveBoard::initializeKingMoves() {
	for (uint8_t s = 0; s < 64; s++) {
		king_moves[s].set(s);
		king_moves[s] |= king_moves[s].clone().left() | king_moves[s].clone().right();
		king_moves[s] |= king_moves[s].clone().up() | king_moves[s].clone().down();
		king_moves[s].clear(s);
	}
}

void MoveBoard::initializeKnightMoves() {
	for (uint8_t s = 0; s < 64; s++) {
		BitBoard bb_vert;
		bb_vert.set(s);
		bb_vert = bb_vert.clone().left() | bb_vert.clone().right();
		bb_vert = bb_vert.clone().up().up() | bb_vert.clone().down().down();

		BitBoard bb_hor;
		bb_hor.set(s);
		bb_hor = bb_hor.clone().up() | bb_hor.clone().down();
		bb_hor = bb_hor.clone().left().left() | bb_hor.clone().right().right();

		knight_moves[s] = bb_vert | bb_hor;
	}
}

void MoveBoard::initializePawnPushes() {
	for (uint8_t s = 0; s < 64; s++) {
		//double push will be handled in move generator
		pawn_pushes[Piece::COLOR_BLACK][s].set(s).down();
		pawn_pushes[Piece::COLOR_WHITE][s].set(s).up();
	}
}

void MoveBoard::initializePawnCaptures() {
	for (uint8_t s = 0; s < 64; s++) {
		BitBoard black_board;
		black_board.set(s);
		pawn_captures[Piece::COLOR_BLACK][s] = black_board.clone().down().left() | black_board.clone().down().right();

		BitBoard white_board;
		white_board.set(s);
		pawn_captures[Piece::COLOR_WHITE][s] = white_board.clone().up().left() | white_board.clone().up().right();
	}
}

void MoveBoard::initializeRays() {
	initializeRayUp();
	initializeRayUpRight();
	initializeRayRight();
	initializeRayDownRight();
	initializeRayDown();
	initializeRayDownLeft();
	initializeRayLeft();
	initializeRayUpLeft();
}

void MoveBoard::initializeRayUp() {
	for (uint8_t s = 0; s < 64; s++) {
		BitBoard board = BitBoard().set(s);
		while (true) {
			board.up();
			if (board.empty())
				break;
			rays[DIR_UP][s] |= board;
		}
	}
}

void MoveBoard::initializeRayUpRight() {
	for (uint8_t s = 0; s < 64; s++) {
		BitBoard board = BitBoard().set(s);
		while (true) {
			board.up().right();
			if (board.empty())
				break;
			rays[DIR_UP_RIGHT][s] |= board;
		}
	}
}

void MoveBoard::initializeRayRight() {
	for (uint8_t s = 0; s < 64; s++) {
		BitBoard board = BitBoard().set(s);
		while (true) {
			board.right();
			if (board.empty())
				break;
			rays[DIR_RIGHT][s] |= board;
		}
	}
}

void MoveBoard::initializeRayDownRight() {
	for (uint8_t s = 0; s < 64; s++) {
		BitBoard board = BitBoard().set(s);
		while (true) {
			board.down().right();
			if (board.empty())
				break;
			rays[DIR_DOWN_RIGHT][s] |= board;
		}
	}
}

void MoveBoard::initializeRayDown() {
	for (uint8_t s = 0; s < 64; s++) {
		BitBoard board = BitBoard().set(s);
		while (true) {
			board.down();
			if (board.empty())
				break;
			rays[DIR_DOWN][s] |= board;
		}
	}
}

void MoveBoard::initializeRayDownLeft() {
	for (uint8_t s = 0; s < 64; s++) {
		BitBoard board = BitBoard().set(s);
		while (true) {
			board.down().left();
			if (board.empty())
				break;
			rays[DIR_DOWN_LEFT][s] |= board;
		}
	}
}

void MoveBoard::initializeRayLeft() {
	for (uint8_t s = 0; s < 64; s++) {
		BitBoard board = BitBoard().set(s);
		while (true) {
			board.left();
			if (board.empty())
				break;
			rays[DIR_LEFT][s] |= board;
		}
	}
}

void MoveBoard::initializeRayUpLeft() {
	for (uint8_t s = 0; s < 64; s++) {
		BitBoard board = BitBoard().set(s);
		while (true) {
			board.up().left();
			if (board.empty())
				break;
			rays[DIR_UP_LEFT][s] |= board;
		}
	}
}

void MoveBoard::initializeCastlingSquares() {
	castling_safe_squares[Constants::CASTLING_WHITE_KINGSIDE] =   0b01110000Ui64;
	castling_empty_squares[Constants::CASTLING_WHITE_KINGSIDE] =  0b01100000Ui64;
	
	castling_safe_squares[Constants::CASTLING_WHITE_QUEENSIDE] =  0b00011100Ui64;
	castling_empty_squares[Constants::CASTLING_WHITE_QUEENSIDE] = 0b00001110Ui64;

	castling_safe_squares[Constants::CASTLING_BLACK_KINGSIDE] =   0b01110000Ui64 << 56;
	castling_empty_squares[Constants::CASTLING_BLACK_KINGSIDE] =  0b01100000Ui64 << 56;
	
	castling_safe_squares[Constants::CASTLING_BLACK_QUEENSIDE] =  0b00011100Ui64 << 56;
	castling_empty_squares[Constants::CASTLING_BLACK_QUEENSIDE] = 0b00001110Ui64 << 56;
}

MoveBoard& MoveBoard::getInstance() {
	static MoveBoard instance;
	return instance;
}

void MoveBoard::initialize() {
	initializeKingMoves();
	initializeKnightMoves();
	initializePawnPushes();
	initializePawnCaptures();
	initializeRays();
	initializeCastlingSquares();
	
	Forward[Piece::COLOR_BLACK] = &BitBoard::down;
	Forward[Piece::COLOR_WHITE] = &BitBoard::up;
}

BitBoard MoveBoard::getKingMoves(const Square square) const {
	return king_moves[square.getValue()];
}

BitBoard MoveBoard::getKnightMoves(const Square square) const {
	return knight_moves[square.getValue()];
}

BitBoard MoveBoard::getPawnPushes(const Square square, uint8_t color) const {
	return pawn_pushes[color][square.getValue()];
}

BitBoard MoveBoard::getPawnCaptures(const Square square, uint8_t color) const {
	return pawn_captures[color][square.getValue()];
}

BitBoard MoveBoard::getRay(const Square square, const uint8_t dir) const {
	return rays[dir][square.getValue()];
}

BitBoard MoveBoard::getCastlingEmptySquares(const int index) const {
	return castling_empty_squares[index];
}

BitBoard MoveBoard::getCastlingSafeSquares(const int index) const {
	return castling_safe_squares[index];
}

MoveBoard::~MoveBoard() {
}
