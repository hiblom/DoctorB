#include "stdafx.h"
#include "MoveGenerator.h"
#include "Square.h"
#include "MoveBoard.h"

using namespace std;

MoveGenerator::MoveGenerator(const Position& generator_position): position(generator_position) {
	uint8_t active_color = position.GetActiveColor();
	uint8_t inactive_color = position.GetActiveColor() ^ 1Ui8;
	for (uint8_t piece_type = 0; piece_type <= 10; piece_type += 2) {
		active_board |= position.GetBitBoard(Piece(piece_type, active_color));
		inactive_board |= position.GetBitBoard(Piece(piece_type, inactive_color));
	}
	combined_board = active_board | inactive_board;
}

MoveGenerator::~MoveGenerator() {
}

void MoveGenerator::GenerateMoves(vector<Move>& moves) const {
	//TODO legality checking (?)
	GeneratePawnMoves(moves);
	GenerateKingMoves(moves);
	GenerateQueenMoves(moves);
	GenerateRookMoves(moves);
	GenerateBishopMoves(moves);
	GenerateKnightMoves(moves);
}

void MoveGenerator::GenerateKingMoves(std::vector<Move>& moves) const {
	uint8_t active_color = position.GetActiveColor();
	vector<Square> from_squares;
	if (!position.GetPieceSquares(Piece(Piece::TYPE_KING, active_color), from_squares))
		return;

	for (Square from_square : from_squares) {
		BitBoard to_board = MoveBoard::GetInstance().GetKingMoves(from_square);
		to_board &= ~active_board; //exclude own pieces

		if (to_board.IsEmpty())
			continue;

		vector<Square> to_squares;
		to_board.GetSquares(to_squares);

		for (Square to_square : to_squares) {
			moves.push_back(Move(from_square, to_square));
		}
	}

	//TODO castling
}

void MoveGenerator::GenerateKnightMoves(std::vector<Move>& moves) const {
	uint8_t active_color = position.GetActiveColor();
	vector<Square> from_squares;
	if (!position.GetPieceSquares(Piece(Piece::TYPE_KNIGHT, active_color), from_squares))
		return;

	for (Square from_square : from_squares) {
		BitBoard to_board = MoveBoard::GetInstance().GetKnightMoves(from_square);
		to_board &= ~active_board; //exclude own pieces

		if (to_board.IsEmpty())
			continue;

		vector<Square> to_squares;
		to_board.GetSquares(to_squares);

		for (Square to_square : to_squares) {
			moves.push_back(Move(from_square, to_square));
		}
	}
}

void MoveGenerator::GeneratePawnMoves(std::vector<Move>& moves) const {
	static const uint8_t DOUBLE_PUSH_RANKS[2] = { 1Ui8, 6Ui8 };

	const Piece active_piece = Piece(Piece::TYPE_PAWN, position.GetActiveColor());

	vector<Square> from_squares;
	if (!position.GetPieceSquares(active_piece, from_squares))
		return;

	BitBoard to_board, to_board_2;
	Square to_square;
	vector<Square> to_squares;
	for (Square from_square : from_squares) {
		//single push
		to_board = MoveBoard::GetInstance().GetPawnPushes(from_square, position.GetActiveColor());
		to_board &= ~combined_board; //exclude all pieces: only silent moves

		if (to_board.GetLowestSquare(to_square)) {
			//TODO promotion!
			moves.push_back(Move(from_square, to_square));

			//double push
			if (from_square.GetY() != DOUBLE_PUSH_RANKS[position.GetActiveColor()])
				continue;

			to_board_2 = MoveBoard::GetInstance().GetPawnPushes(to_square, position.GetActiveColor());
			to_board_2 &= ~combined_board; //exclude all pieces: only silent moves

			if (to_board_2.GetLowestSquare(to_square)) {
				moves.push_back(Move(from_square, to_square));
				//TODO set EP square
			}
		}

		//captures
		//TODO EP capture
		to_board = MoveBoard::GetInstance().GetPawnCaptures(from_square, position.GetActiveColor());
		to_board &= inactive_board; //only captures
		to_squares.clear();
		to_board.GetSquares(to_squares);
		for (Square to_square : to_squares) {
			//TODO promotions
			moves.push_back(Move(from_square, to_square));
		}
	}
}

void MoveGenerator::GenerateRookMoves(std::vector<Move>& moves) const {
	const Piece active_piece = Piece(Piece::TYPE_ROOK, position.GetActiveColor());

	vector<Square> from_squares;
	if (!position.GetPieceSquares(active_piece, from_squares))
		return;

	for (Square from_square : from_squares) {
		GenerateRayMoves(from_square, MoveBoard::DIR_UP, &BitBoard::GetLowestSquare, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_RIGHT, &BitBoard::GetLowestSquare, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_DOWN, &BitBoard::GetHighestSquare, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_LEFT, &BitBoard::GetHighestSquare, moves);
	}
}

void MoveGenerator::GenerateBishopMoves(std::vector<Move>& moves) const {
	const Piece active_piece = Piece(Piece::TYPE_BISHOP, position.GetActiveColor());

	vector<Square> from_squares;
	if (!position.GetPieceSquares(active_piece, from_squares))
		return;

	for (Square from_square : from_squares) {
		GenerateRayMoves(from_square, MoveBoard::DIR_UP_RIGHT, &BitBoard::GetLowestSquare, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_DOWN_RIGHT, &BitBoard::GetHighestSquare, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_DOWN_LEFT, &BitBoard::GetHighestSquare, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_UP_LEFT, &BitBoard::GetLowestSquare, moves);
	}
}

void MoveGenerator::GenerateQueenMoves(std::vector<Move>& moves) const {
	const Piece active_piece = Piece(Piece::TYPE_QUEEN, position.GetActiveColor());

	vector<Square> from_squares;
	if (!position.GetPieceSquares(active_piece, from_squares))
		return;

	for (Square from_square : from_squares) {
		//rook moves
		GenerateRayMoves(from_square, MoveBoard::DIR_UP, &BitBoard::GetLowestSquare, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_RIGHT, &BitBoard::GetLowestSquare, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_DOWN, &BitBoard::GetHighestSquare, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_LEFT, &BitBoard::GetHighestSquare, moves);
		//bishop moves
		GenerateRayMoves(from_square, MoveBoard::DIR_UP_RIGHT, &BitBoard::GetLowestSquare, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_DOWN_RIGHT, &BitBoard::GetHighestSquare, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_DOWN_LEFT, &BitBoard::GetHighestSquare, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_UP_LEFT, &BitBoard::GetLowestSquare, moves);
	}
}

void MoveGenerator::GenerateRayMoves(const Square& from_square, const int dir, bool(BitBoard::*find_nearest_square)(Square&) const, std::vector<Move>& moves) const {
	BitBoard move_board;
	BitBoard forward_ray_board = MoveBoard::GetInstance().GetRay(from_square, dir);
	BitBoard forward_ray_intersect = forward_ray_board & combined_board;
	if (!forward_ray_intersect.IsEmpty()) {
		Square nearest_square;
		if ((forward_ray_intersect.*find_nearest_square)(nearest_square)) {
			int reverse_dir = (dir + 4) % 8;
			BitBoard reverse_ray_board = MoveBoard::GetInstance().GetRay(nearest_square, reverse_dir);
			move_board |= forward_ray_board & reverse_ray_board;
			BitBoard nearest_square_board;
			nearest_square_board.Set(nearest_square.GetValue());
			move_board |= nearest_square_board & inactive_board; //add capture
		}
	}
	else {
		move_board = forward_ray_board;
	}

	vector<Square> to_squares;
	move_board.GetSquares(to_squares);
	for (Square to_square : to_squares) {
		moves.push_back(Move(from_square, to_square));
	}
}


