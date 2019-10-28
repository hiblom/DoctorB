#include "stdafx.h"
#include "MoveGenerator.h"
#include "Square.h"
#include "MoveBoard.h"
#include "Constants.h"

using namespace std;

MoveGenerator::MoveGenerator(const Position& generator_position) : position(generator_position) {
	uint8_t active_color = position.GetActiveColor();
	uint8_t inactive_color = position.GetActiveColor() ^ 1Ui8;
	
	//previous loop unrolled lessen the number of loops
	active_board = position.GetBitBoard(Piece(Piece::TYPE_PAWN, active_color));
	inactive_board = position.GetBitBoard(Piece(Piece::TYPE_PAWN, inactive_color));

	active_board |= position.GetBitBoard(Piece(Piece::TYPE_KING, active_color));
	inactive_board |= position.GetBitBoard(Piece(Piece::TYPE_KING, inactive_color));

	active_board |= position.GetBitBoard(Piece(Piece::TYPE_QUEEN, active_color));
	inactive_board |= position.GetBitBoard(Piece(Piece::TYPE_QUEEN, inactive_color));

	active_board |= position.GetBitBoard(Piece(Piece::TYPE_ROOK, active_color));
	inactive_board |= position.GetBitBoard(Piece(Piece::TYPE_ROOK, inactive_color));

	active_board |= position.GetBitBoard(Piece(Piece::TYPE_BISHOP, active_color));
	inactive_board |= position.GetBitBoard(Piece(Piece::TYPE_BISHOP, inactive_color));

	active_board |= position.GetBitBoard(Piece(Piece::TYPE_KNIGHT, active_color));
	inactive_board |= position.GetBitBoard(Piece(Piece::TYPE_KNIGHT, inactive_color));

	combined_board = active_board | inactive_board;
}

MoveGenerator::~MoveGenerator() {
}

void MoveGenerator::GenerateMoves(vector<Move>& moves) const {
	//generate "danger squares" => empty squares that are attacked by enemy when excluding own king
	//king cannot move into these squares
	//we can generate this danger board without serializing to a square vector

	//seperate move generation for check evasion and double check evasion
	//count number of pieces attacking the active king

	//in case of double check the only legal moves are king moves
	//in case of single check we can either move the king, capture the checker, or block the checker

	//pinned pieces restricted in their moves

	//keep into account some gotcha's like en passant discovered check



	GeneratePawnMoves(moves);
	GenerateKingMoves(moves);
	GenerateQueenMoves(moves);
	GenerateRookMoves(moves);
	GenerateBishopMoves(moves);
	GenerateKnightMoves(moves);
}

void MoveGenerator::GenerateKingMoves(std::vector<Move>& moves) const {
	static const int CASTLING_KINGSIDE[2] = { Constants::CASTLING_WHITE_KINGSIDE, Constants::CASTLING_BLACK_KINGSIDE };
	static const int CASTLING_QUEENSIDE[2] = { Constants::CASTLING_WHITE_QUEENSIDE, Constants::CASTLING_BLACK_QUEENSIDE };
	static const int KING_SQUARE[2] = { Square::E1, Square::E8 };
	static const int ROOK_SQUARE_KINGSIDE[2] = { Square::G1, Square::G8 };
	static const int ROOK_SQUARE_QUEENSIDE[2] = { Square::C1, Square::C8 };

	uint8_t active_color = position.GetActiveColor();
	Piece active_piece = Piece(Piece::TYPE_KING, active_color);
	Square from_square;

	if (!position.GetPieceSquare(active_piece, from_square))
		return;

	BitBoard to_board = MoveBoard::GetInstance().GetKingMoves(from_square);
	to_board &= ~active_board; //exclude own pieces

	vector<Square> to_squares;
	to_board.GetSquares(to_squares);

	for (Square to_square : to_squares) {
		moves.push_back(Move(active_piece, from_square, to_square));
	}

	//castling
	if (CanCastle(CASTLING_KINGSIDE[active_color]))
		moves.push_back(Move(active_piece, Square(KING_SQUARE[active_color]), Square(ROOK_SQUARE_KINGSIDE[active_color])).SetCastling());

	if (CanCastle(CASTLING_QUEENSIDE[active_color]))
		moves.push_back(Move(active_piece, Square(KING_SQUARE[active_color]), Square(ROOK_SQUARE_QUEENSIDE[active_color])).SetCastling());
}

void MoveGenerator::GenerateKnightMoves(std::vector<Move>& moves) const {
	uint8_t active_color = position.GetActiveColor();
	Piece active_piece = Piece(Piece::TYPE_KNIGHT, active_color);
	vector<Square> from_squares;
	if (!position.GetPieceSquares(active_piece, from_squares))
		return;

	for (Square from_square : from_squares) {
		BitBoard to_board = MoveBoard::GetInstance().GetKnightMoves(from_square);
		to_board &= ~active_board; //exclude own pieces

		if (to_board.Empty())
			continue;

		vector<Square> to_squares;
		to_board.GetSquares(to_squares);

		for (Square to_square : to_squares) {
			moves.push_back(Move(active_piece, from_square, to_square));
		}
	}
}

void MoveGenerator::GeneratePawnMoves(std::vector<Move>& moves) const {
	static const uint8_t DOUBLE_PUSH_RANKS[2] = { 1Ui8, 6Ui8 };
	static const uint8_t PROMOTION_RANKS[2] = { 6Ui8, 1Ui8 };

	uint8_t active_color = position.GetActiveColor();
	const Piece active_piece = Piece(Piece::TYPE_PAWN, position.GetActiveColor());

	vector<Square> from_squares;
	if (!position.GetPieceSquares(active_piece, from_squares))
		return;

	BitBoard to_board, to_board_2;
	Square to_square;
	vector<Square> to_squares;
	for (Square from_square : from_squares) {
		//single push
		to_board = MoveBoard::GetInstance().GetPawnPushes(from_square, active_color);
		to_board &= ~combined_board; //exclude all pieces: only silent moves

		if (to_board.GetLowestSquare(to_square)) {
			if (from_square.GetY() == PROMOTION_RANKS[active_color]) {
				moves.push_back(Move(active_piece, from_square, to_square, Piece(Piece::TYPE_QUEEN, active_color)));
				moves.push_back(Move(active_piece, from_square, to_square, Piece(Piece::TYPE_BISHOP, active_color)));
				moves.push_back(Move(active_piece, from_square, to_square, Piece(Piece::TYPE_ROOK, active_color)));
				moves.push_back(Move(active_piece, from_square, to_square, Piece(Piece::TYPE_KNIGHT, active_color)));
			}
			else {
				moves.push_back(Move(active_piece, from_square, to_square));

				//double push
				if (from_square.GetY() == DOUBLE_PUSH_RANKS[active_color]) {
					to_board_2 = MoveBoard::GetInstance().GetPawnPushes(to_square, active_color);
					to_board_2 &= ~combined_board; //exclude all pieces: only silent moves

					if (to_board_2.GetLowestSquare(to_square)) {
						moves.push_back(Move(active_piece, from_square, to_square).SetDoublePush());
					}
				}
			}
		}

		//captures
		to_board = MoveBoard::GetInstance().GetPawnCaptures(from_square, active_color);
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
			if (from_square.GetY() == PROMOTION_RANKS[active_color]) {
				moves.push_back(Move(active_piece, from_square, to_square, Piece(Piece::TYPE_QUEEN, active_color)));
				moves.push_back(Move(active_piece, from_square, to_square, Piece(Piece::TYPE_BISHOP, active_color)));
				moves.push_back(Move(active_piece, from_square, to_square, Piece(Piece::TYPE_ROOK, active_color)));
				moves.push_back(Move(active_piece, from_square, to_square, Piece(Piece::TYPE_KNIGHT, active_color)));
			}
			else {
				Move move = Move(active_piece, from_square, to_square);
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
		GenerateRayMoves(from_square, MoveBoard::DIR_UP, &BitBoard::GetLowestSquare, active_piece, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_RIGHT, &BitBoard::GetLowestSquare, active_piece, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_DOWN, &BitBoard::GetHighestSquare, active_piece, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_LEFT, &BitBoard::GetHighestSquare, active_piece, moves);
	}
}

void MoveGenerator::GenerateBishopMoves(std::vector<Move>& moves) const {
	const Piece active_piece = Piece(Piece::TYPE_BISHOP, position.GetActiveColor());

	vector<Square> from_squares;
	if (!position.GetPieceSquares(active_piece, from_squares))
		return;

	for (Square from_square : from_squares) {
		GenerateRayMoves(from_square, MoveBoard::DIR_UP_RIGHT, &BitBoard::GetLowestSquare, active_piece, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_DOWN_RIGHT, &BitBoard::GetHighestSquare, active_piece, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_DOWN_LEFT, &BitBoard::GetHighestSquare, active_piece, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_UP_LEFT, &BitBoard::GetLowestSquare, active_piece, moves);
	}
}

void MoveGenerator::GenerateQueenMoves(std::vector<Move>& moves) const {
	const Piece active_piece = Piece(Piece::TYPE_QUEEN, position.GetActiveColor());

	vector<Square> from_squares;
	if (!position.GetPieceSquares(active_piece, from_squares))
		return;

	for (Square from_square : from_squares) {
		//rook moves
		GenerateRayMoves(from_square, MoveBoard::DIR_UP, &BitBoard::GetLowestSquare, active_piece, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_RIGHT, &BitBoard::GetLowestSquare, active_piece, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_DOWN, &BitBoard::GetHighestSquare, active_piece, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_LEFT, &BitBoard::GetHighestSquare, active_piece, moves);
		//bishop moves
		GenerateRayMoves(from_square, MoveBoard::DIR_UP_RIGHT, &BitBoard::GetLowestSquare, active_piece, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_DOWN_RIGHT, &BitBoard::GetHighestSquare, active_piece, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_DOWN_LEFT, &BitBoard::GetHighestSquare, active_piece, moves);
		GenerateRayMoves(from_square, MoveBoard::DIR_UP_LEFT, &BitBoard::GetLowestSquare, active_piece, moves);
	}
}

bool MoveGenerator::IsCheck(uint8_t color) {
	vector<Square> squares;
	if (position.GetPieceSquares(Piece(Piece::TYPE_KING, color), squares)) {
		if (squares.size() != 1)
			return false;

		return IsSquareAttacked(squares[0], color ^ 1Ui8);
	}
	return false;
}


bool MoveGenerator::SetMoveFlags(Move& move) {
	Piece piece;
	if (!position.GetPiece(move.GetSquareFrom(), piece))
		return false;

	//piece
	move.SetPiece(piece);

	//castling
	if (piece.GetType() == Piece::TYPE_KING) {
		if (move.GetSquareFrom() == Square(Square::E1) && (move.GetSquareTo() == Square(Square::C1) || move.GetSquareTo() == Square(Square::G1)) ||
			move.GetSquareFrom() == Square(Square::E8) && (move.GetSquareTo() == Square(Square::C8) || move.GetSquareTo() == Square(Square::G8))) {
			move.SetCastling();
			return true;
		}
	}
	else if (piece.GetType() == Piece::TYPE_PAWN) {
		//ep capture
		Square ep_square;
		if (position.GetEpSquare(ep_square)) {
			if (ep_square == move.GetSquareTo()) {
				move.SetEpCapture();
				return true;
			}
		}

		//double push
		if (move.GetSquareFrom().GetY() == 1 && move.GetSquareTo().GetY() == 3 ||
			move.GetSquareFrom().GetY() == 6 && move.GetSquareTo().GetY() == 4) {
			move.SetDoublePush();
			return true;
		}
	}

	//note: promotion is set automatically when parsing the move string
	return true;
}

void MoveGenerator::GenerateRayMoves(const Square& from_square, const uint8_t dir, bool(BitBoard::*find_nearest_square)(Square&) const, const Piece active_piece , std::vector<Move>& moves) const {
	//BitBoard move_board;
	BitBoard move_board = MoveBoard::GetInstance().GetRay(from_square, dir);
	BitBoard intersect_board = move_board & combined_board;

	if (intersect_board.NotEmpty()) {
		Square nearest_square;
		(intersect_board.*find_nearest_square)(nearest_square);
		//slight change after reading https://www.chessprogramming.org/Classical_Approach
		move_board ^= MoveBoard::GetInstance().GetRay(nearest_square, dir);
		move_board &= ~active_board; //subtract own pieces
	}

	vector<Square> to_squares;
	move_board.GetSquares(to_squares);
	for (Square to_square : to_squares) {
		moves.push_back(Move(active_piece, from_square, to_square));
	}
}

bool MoveGenerator::IsSquareAttacked(const Square& square, const uint8_t attacking_color) const {
	uint8_t defending_color = attacking_color ^ 1Ui8;

	//attacked by pawn
	//pretend we are a pawn, can we capture opponent's pawn?
	BitBoard pawn_capture_board = MoveBoard::GetInstance().GetPawnCaptures(square, defending_color);
	BitBoard attacking_pawn_board = position.GetBitBoard(Piece(Piece::TYPE_PAWN, attacking_color));
	if ((pawn_capture_board & attacking_pawn_board).NotEmpty())
		return true;

	//attacked by knight
	//pretend we are a knight, can we capture opponent's knight?
	BitBoard knight_capture_board = MoveBoard::GetInstance().GetKnightMoves(square);
	BitBoard attacking_knight_board = position.GetBitBoard(Piece(Piece::TYPE_KNIGHT, attacking_color));
	if ((knight_capture_board & attacking_knight_board).NotEmpty())
		return true;

	//attacked by king
	//pretend we are a king, can we capture opponent's king?
	BitBoard king_capture_board = MoveBoard::GetInstance().GetKingMoves(square);
	BitBoard attacking_king_board = position.GetBitBoard(Piece(Piece::TYPE_KING, attacking_color));
	if ((king_capture_board & attacking_king_board).NotEmpty())
		return true;

	//attacked by sliding pieces?
	return
		IsAttackedFromDirection(square, MoveBoard::DIR_UP, &BitBoard::GetLowestSquare, Piece::TYPE_ROOK, attacking_color) ||
		IsAttackedFromDirection(square, MoveBoard::DIR_DOWN, &BitBoard::GetHighestSquare, Piece::TYPE_ROOK, attacking_color) ||
		IsAttackedFromDirection(square, MoveBoard::DIR_RIGHT, &BitBoard::GetLowestSquare, Piece::TYPE_ROOK, attacking_color) ||
		IsAttackedFromDirection(square, MoveBoard::DIR_LEFT, &BitBoard::GetHighestSquare, Piece::TYPE_ROOK, attacking_color) ||
		IsAttackedFromDirection(square, MoveBoard::DIR_UP_RIGHT, &BitBoard::GetLowestSquare, Piece::TYPE_BISHOP, attacking_color) ||
		IsAttackedFromDirection(square, MoveBoard::DIR_DOWN_LEFT, &BitBoard::GetHighestSquare, Piece::TYPE_BISHOP, attacking_color) ||
		IsAttackedFromDirection(square, MoveBoard::DIR_UP_LEFT, &BitBoard::GetLowestSquare, Piece::TYPE_BISHOP, attacking_color) ||
		IsAttackedFromDirection(square, MoveBoard::DIR_DOWN_RIGHT, &BitBoard::GetHighestSquare, Piece::TYPE_BISHOP, attacking_color);
}

bool MoveGenerator::IsAttackedFromDirection(const Square& square, const uint8_t dir, bool(BitBoard::*find_nearest_square)(Square&) const, const uint8_t rook_or_bishop_type, const uint8_t attacking_color) const {
	BitBoard forward_ray_board = MoveBoard::GetInstance().GetRay(square.GetValue(), dir);
	BitBoard forward_ray_intersect = forward_ray_board & combined_board;

	if (forward_ray_intersect.Empty())
		return false;

	Square nearest_square;
	(forward_ray_intersect.*find_nearest_square)(nearest_square);
	BitBoard nearest_square_board = BitBoard().Set(nearest_square.GetValue());
	BitBoard possible_attackers = position.GetBitBoard(Piece(Piece::TYPE_QUEEN, attacking_color)) | position.GetBitBoard(Piece(rook_or_bishop_type, attacking_color));
	return (nearest_square_board & possible_attackers).NotEmpty();
}

bool MoveGenerator::CanCastle(const int castling_index) const {
	//check castling rights
	if (!position.GetCastlingStatus(castling_index))
		return false;

	//squares between king and rook must be empty
	if ((MoveBoard::GetInstance().GetCastlingEmptySquares(castling_index) & combined_board).NotEmpty())
		return false;

	//squares king passes through (including current position) may not be attacked by opponent
	vector<Square> safe_squares;
	MoveBoard::GetInstance().GetCastlingSafeSquares(castling_index).GetSquares(safe_squares);
	for (Square square : safe_squares) {
		if (IsSquareAttacked(square, position.GetActiveColor() ^ 1Ui8))
			return false;
	}

	return true;
}

