#include "stdafx.h"
#include "MoveGenerator.h"
#include "Square.h"
#include "MoveBoard.h"
#include "Constants.h"

using namespace std;

MoveGenerator::MoveGenerator(const Position& generator_position) : position(generator_position) {
	active_color = position.GetActiveColor();
	inactive_color = position.GetActiveColor() ^ 1Ui8;
	
	//previous loop unrolled to lessen the number of loops
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

void MoveGenerator::GenerateMoves(vector<Move>& moves) {
	GenerateKingMoves(moves);
	GeneratePawnMoves(moves);
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

	Piece active_piece = Piece(Piece::TYPE_KING, active_color);
	Square from_square;

	if (!position.GetPieceSquare(active_piece, from_square))
		return;

	BitBoard to_board = MoveBoard::GetInstance().GetKingMoves(from_square);
	to_board &= ~active_board; //exclude own pieces

	Square to_square;
	while(to_board.ConsumeLowestSquare(to_square)) {
		moves.push_back(Move(active_piece, from_square, to_square));
	}

	//castling
	if (CanCastle(CASTLING_KINGSIDE[active_color]))
		moves.push_back(Move(active_piece, Square(KING_SQUARE[active_color]), Square(ROOK_SQUARE_KINGSIDE[active_color])).SetCastling());

	if (CanCastle(CASTLING_QUEENSIDE[active_color]))
		moves.push_back(Move(active_piece, Square(KING_SQUARE[active_color]), Square(ROOK_SQUARE_QUEENSIDE[active_color])).SetCastling());
}

void MoveGenerator::GenerateKnightMoves(std::vector<Move>& moves) const {
	Piece active_piece = Piece(Piece::TYPE_KNIGHT, active_color);
	BitBoard piece_board = position.GetBitBoard(active_piece);

	Square from_square;
	while(piece_board.ConsumeLowestSquare(from_square)) {
		BitBoard to_board = MoveBoard::GetInstance().GetKnightMoves(from_square);
		to_board &= ~active_board; //exclude own pieces

		Square to_square;
		while(to_board.ConsumeLowestSquare(to_square)) {
			moves.push_back(Move(active_piece, from_square, to_square));
		}
	}
}

void MoveGenerator::GeneratePawnMoves(std::vector<Move>& moves) const {
	static const uint8_t DOUBLE_PUSH_RANKS[2] = { 1Ui8, 6Ui8 };
	static const uint8_t PROMOTION_RANKS[2] = { 6Ui8, 1Ui8 };

	const Piece active_piece = Piece(Piece::TYPE_PAWN, position.GetActiveColor());

	BitBoard pawn_board = position.GetBitBoard(active_piece);
	BitBoard to_board, to_board_2;
	Square from_square, to_square;
	while(pawn_board.ConsumeLowestSquare(from_square)) {
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

		while(to_board.ConsumeLowestSquare(to_square)) {
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
	const Piece piece = Piece(Piece::TYPE_ROOK, position.GetActiveColor());

	BitBoard piece_board = position.GetBitBoard(piece);
	Square piece_square;
	while(piece_board.ConsumeLowestSquare(piece_square)) {
		GenerateRayMoves(piece_square, MoveBoard::DIR_UP, &BitBoard::GetLowestSquare, piece, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_RIGHT, &BitBoard::GetLowestSquare, piece, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_DOWN, &BitBoard::GetHighestSquare, piece, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_LEFT, &BitBoard::GetHighestSquare, piece, moves);
	}
}

void MoveGenerator::GenerateBishopMoves(std::vector<Move>& moves) const {
	const Piece piece = Piece(Piece::TYPE_BISHOP, position.GetActiveColor());

	BitBoard piece_board = position.GetBitBoard(piece);
	Square piece_square;
	while (piece_board.ConsumeLowestSquare(piece_square)) {
		GenerateRayMoves(piece_square, MoveBoard::DIR_UP_RIGHT, &BitBoard::GetLowestSquare, piece, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_DOWN_RIGHT, &BitBoard::GetHighestSquare, piece, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_DOWN_LEFT, &BitBoard::GetHighestSquare, piece, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_UP_LEFT, &BitBoard::GetLowestSquare, piece, moves);
	}
}

void MoveGenerator::GenerateQueenMoves(std::vector<Move>& moves) const {
	const Piece piece = Piece(Piece::TYPE_QUEEN, position.GetActiveColor());
	
	BitBoard piece_board = position.GetBitBoard(piece);
	Square piece_square;
	while (piece_board.ConsumeLowestSquare(piece_square)) {
		//rook moves
		GenerateRayMoves(piece_square, MoveBoard::DIR_UP, &BitBoard::GetLowestSquare, piece, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_RIGHT, &BitBoard::GetLowestSquare, piece, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_DOWN, &BitBoard::GetHighestSquare, piece, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_LEFT, &BitBoard::GetHighestSquare, piece, moves);
		//bishop moves
		GenerateRayMoves(piece_square, MoveBoard::DIR_UP_RIGHT, &BitBoard::GetLowestSquare, piece, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_DOWN_RIGHT, &BitBoard::GetHighestSquare, piece, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_DOWN_LEFT, &BitBoard::GetHighestSquare, piece, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_UP_LEFT, &BitBoard::GetLowestSquare, piece, moves);
	}
}

bool MoveGenerator::IsCheck(uint8_t color) const {
	BitBoard king_board = position.GetBitBoard(Piece(Piece::TYPE_KING, color));
	Square square;
	if (king_board.GetLowestSquare(square)) {
		return IsSquareAttacked(square, color ^ 1Ui8);
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

	Square to_square;
	while(move_board.ConsumeLowestSquare(to_square)) {
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

bool MoveGenerator::IsAttackedFromDirection(const Square square, const uint8_t dir, bool(BitBoard::*find_nearest_square)(Square&) const, const uint8_t rook_or_bishop_type, const uint8_t attacking_color) const {
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

	if (IsCheck(active_color))
		return false;

	//squares king passes through may not be attacked by opponent
	BitBoard safe_board = MoveBoard::GetInstance().GetCastlingSafeSquares(castling_index);
	Square square;
	while (safe_board.ConsumeLowestSquare(square)) {
		if (IsSquareAttacked(square, inactive_color))
			return false;
	}
	return true;
}
