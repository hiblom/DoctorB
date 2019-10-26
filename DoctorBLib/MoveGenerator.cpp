#include "stdafx.h"
#include "MoveGenerator.h"
#include "Square.h"
#include "MoveBoard.h"
#include "Constants.h"

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

		if (to_board.Empty())
			continue;

		vector<Square> to_squares;
		to_board.GetSquares(to_squares);

		for (Square to_square : to_squares) {
			moves.push_back(Move(from_square, to_square));
		}
	}

	//castling
	if (active_color == Piece::COLOR_WHITE) {
		if (CanCastle(Constants::CASTLING_WHITE_KINGSIDE))
			moves.push_back(Move(Square(Square::E1), Square(Square::G1)).SetCastling());

		if (CanCastle(Constants::CASTLING_WHITE_QUEENSIDE))
			moves.push_back(Move(Square(Square::E1), Square(Square::C1)).SetCastling());
	}
	else {
		if (CanCastle(Constants::CASTLING_BLACK_KINGSIDE))
			moves.push_back(Move(Square(Square::E8), Square(Square::G8)).SetCastling());

		if (CanCastle(Constants::CASTLING_BLACK_QUEENSIDE))
			moves.push_back(Move(Square(Square::E8), Square(Square::C8)).SetCastling());
	}
}

void MoveGenerator::GenerateKnightMoves(std::vector<Move>& moves) const {
	uint8_t active_color = position.GetActiveColor();
	vector<Square> from_squares;
	if (!position.GetPieceSquares(Piece(Piece::TYPE_KNIGHT, active_color), from_squares))
		return;

	for (Square from_square : from_squares) {
		BitBoard to_board = MoveBoard::GetInstance().GetKnightMoves(from_square);
		to_board &= ~active_board; //exclude own pieces

		if (to_board.Empty())
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
	static const uint8_t PROMOTION_RANKS[2] = { 6Ui8, 1Ui8 };

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
			if (from_square.GetY() == PROMOTION_RANKS[position.GetActiveColor()]) {
				moves.push_back(Move(from_square, to_square, Piece(Piece::TYPE_QUEEN, position.GetActiveColor())));
				moves.push_back(Move(from_square, to_square, Piece(Piece::TYPE_BISHOP, position.GetActiveColor())));
				moves.push_back(Move(from_square, to_square, Piece(Piece::TYPE_ROOK, position.GetActiveColor())));
				moves.push_back(Move(from_square, to_square, Piece(Piece::TYPE_KNIGHT, position.GetActiveColor())));
			}
			else {
				moves.push_back(Move(from_square, to_square));

				//double push
				if (from_square.GetY() == DOUBLE_PUSH_RANKS[position.GetActiveColor()]) {
					to_board_2 = MoveBoard::GetInstance().GetPawnPushes(to_square, position.GetActiveColor());
					to_board_2 &= ~combined_board; //exclude all pieces: only silent moves

					if (to_board_2.GetLowestSquare(to_square)) {
						moves.push_back(Move(from_square, to_square).SetDoublePush());
					}
				}
			}
		}

		//captures
		to_board = MoveBoard::GetInstance().GetPawnCaptures(from_square, position.GetActiveColor());
		BitBoard pawn_targets = BitBoard(inactive_board);
		
		//add EP square to list of targets for pawn captures
		Square ep_square;
		bool has_ep_square = position.GetEpSquare(ep_square);
		if (has_ep_square) {
			pawn_targets |= BitBoard().Set(ep_square.GetValue());
		}

		to_board &= pawn_targets; //only captures
		to_squares.clear();
		to_board.GetSquares(to_squares);
		for (Square to_square : to_squares) {
			if (from_square.GetY() == PROMOTION_RANKS[position.GetActiveColor()]) {
				moves.push_back(Move(from_square, to_square, Piece(Piece::TYPE_QUEEN, position.GetActiveColor())));
				moves.push_back(Move(from_square, to_square, Piece(Piece::TYPE_BISHOP, position.GetActiveColor())));
				moves.push_back(Move(from_square, to_square, Piece(Piece::TYPE_ROOK, position.GetActiveColor())));
				moves.push_back(Move(from_square, to_square, Piece(Piece::TYPE_KNIGHT, position.GetActiveColor())));
			}
			else {
				Move move = Move(from_square, to_square);
				if (has_ep_square && to_square == ep_square) {
					move.SetEpCapture();
				}
				moves.push_back(move);
			}
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
	if (forward_ray_intersect.NotEmpty()) {
		Square nearest_square;
		if ((forward_ray_intersect.*find_nearest_square)(nearest_square)) {
			int reverse_dir = (dir + 4) % 8;
			BitBoard reverse_ray_board = MoveBoard::GetInstance().GetRay(nearest_square, reverse_dir);
			move_board |= forward_ray_board & reverse_ray_board;
			BitBoard nearest_square_board = BitBoard().Set(nearest_square.GetValue());
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

bool MoveGenerator::IsSquareAttacked(const Square& square) const {
	uint8_t active_color = position.GetActiveColor();
	uint8_t inactive_color = active_color ^ 1Ui8;
	
	//attacked by pawn
	//pretend we are a pawn, can we capture opponent's pawn?
	//note: does not take into account EP square
	BitBoard pawn_capture_board = MoveBoard::GetInstance().GetPawnCaptures(square, active_color);
	BitBoard inactive_pawn_board = position.GetBitBoard(Piece(Piece::TYPE_PAWN, inactive_color));
	if ((pawn_capture_board & inactive_pawn_board).NotEmpty())
		return true;

	//attacked by knight
	//pretend we are a knight, can we capture opponent's knight?
	BitBoard knight_capture_board = MoveBoard::GetInstance().GetKnightMoves(square);
	BitBoard inactive_knight_board = position.GetBitBoard(Piece(Piece::TYPE_KNIGHT, inactive_color));
	if ((knight_capture_board & inactive_knight_board).NotEmpty())
		return true;

	//attacked by king
	//pretend we are a king, can we capture opponent's king?
	BitBoard king_capture_board = MoveBoard::GetInstance().GetKingMoves(square);
	BitBoard inactive_king_board = position.GetBitBoard(Piece(Piece::TYPE_KING, inactive_color));
	if ((king_capture_board & inactive_king_board).NotEmpty())
		return true;

	//attacked by sliding pieces?
	return
		IsAttackedFromDirection(square, MoveBoard::DIR_UP, &BitBoard::GetLowestSquare, Piece::TYPE_ROOK) ||
		IsAttackedFromDirection(square, MoveBoard::DIR_UP_RIGHT, &BitBoard::GetLowestSquare, Piece::TYPE_BISHOP) ||
		IsAttackedFromDirection(square, MoveBoard::DIR_RIGHT, &BitBoard::GetLowestSquare, Piece::TYPE_ROOK) ||
		IsAttackedFromDirection(square, MoveBoard::DIR_DOWN_RIGHT, &BitBoard::GetHighestSquare, Piece::TYPE_BISHOP) ||
		IsAttackedFromDirection(square, MoveBoard::DIR_DOWN, &BitBoard::GetHighestSquare, Piece::TYPE_ROOK) ||
		IsAttackedFromDirection(square, MoveBoard::DIR_DOWN_LEFT, &BitBoard::GetHighestSquare, Piece::TYPE_BISHOP) ||
		IsAttackedFromDirection(square, MoveBoard::DIR_LEFT, &BitBoard::GetHighestSquare, Piece::TYPE_ROOK) ||
		IsAttackedFromDirection(square, MoveBoard::DIR_UP_LEFT, &BitBoard::GetLowestSquare, Piece::TYPE_BISHOP);
}

bool MoveGenerator::IsAttackedFromDirection(const Square& square, const int dir, bool(BitBoard::*find_nearest_square)(Square&) const, const uint8_t rook_or_bishop_type) const {
	BitBoard forward_ray_board = MoveBoard::GetInstance().GetRay(square.GetValue(), dir);
	BitBoard forward_ray_intersect = forward_ray_board & combined_board;
	
	if (forward_ray_intersect.Empty())
		return false;

	Square nearest_square;
	if ((forward_ray_intersect.*find_nearest_square)(nearest_square)) {
		BitBoard nearest_square_board = BitBoard().Set(nearest_square.GetValue());

		uint8_t inactive_color = position.GetActiveColor() ^ 1Ui8;
		BitBoard possible_attackers = position.GetBitBoard(Piece(Piece::TYPE_QUEEN, inactive_color)) | position.GetBitBoard(Piece(rook_or_bishop_type, inactive_color));
		return (nearest_square_board & possible_attackers).NotEmpty();
	}
	return false;
}

bool MoveGenerator::CanCastle(const int castling_index) const {
	//check castling rights
	if (!position.GetCastlingStatus(castling_index))
		return false;

	//squares between empty king and rook must be empty
	if ((MoveBoard::GetInstance().GetCastlingEmptySquares(castling_index) & combined_board).NotEmpty())
		return false;
	
	//squares king passes through (including current position) may not be attacked by opponent
	vector<Square> safe_squares;
	MoveBoard::GetInstance().GetCastlingSafeSquares(castling_index).GetSquares(safe_squares);
	for (Square square : safe_squares) {
		if (IsSquareAttacked(square))
			return false;
	}

	return true;
}

