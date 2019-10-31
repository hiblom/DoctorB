#include "stdafx.h"
#include "MoveGenerator2.h"
#include "Square.h"
#include "MoveBoard.h"
#include "Constants.h"

using namespace std;

MoveGenerator2::MoveGenerator2(const Position& generator_position) : position(generator_position) {
	active_color = position.GetActiveColor();
	inactive_color = position.GetActiveColor() ^ 1Ui8;
	checker_count = 0;

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

	//some initialization
	GenerateDangerBoard(); //mark the squares that are not accessible for the king
	GenerateCheckInfo(); //count the number of checkers and generate block board
	GeneratePinInfo(); //mark pinned pieces and pin rays along which they can move
}

MoveGenerator2::~MoveGenerator2() {
}

void MoveGenerator2::GenerateMoves(vector<Move>& moves) {
	//generate "danger squares" => empty squares that are attacked by enemy when excluding own king
	//king cannot move into these squares
	//we can generate this danger board without serializing to a square vector

	//seperate move generation for check evasion and double check evasion
	//count number of pieces attacking the active king

	//in case of double check the only legal moves are king moves
	//in case of single check we can either move the king, capture the checker, or block the checker

	//pinned pieces restricted in their moves

	//keep into account some gotcha's like en passant discovered check

	GenerateKingMoves(moves);
	if (checker_count >= 2)
		return;

	GeneratePawnMoves(moves);
	GenerateQueenMoves(moves);
	GenerateRookMoves(moves);
	GenerateBishopMoves(moves);
	GenerateKnightMoves(moves);
}

void MoveGenerator2::GenerateKingMoves(std::vector<Move>& moves) const {
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
	to_board &= ~danger_board; //exclude danger board

	Square to_square;
	while (to_board.ConsumeLowestSquare(to_square)) {
		moves.push_back(Move(active_piece, from_square, to_square));
	}

	//castling
	if (checker_count == 0) {
		if (CanCastle(CASTLING_KINGSIDE[active_color]))
			moves.push_back(Move(active_piece, Square(KING_SQUARE[active_color]), Square(ROOK_SQUARE_KINGSIDE[active_color])).SetCastling());

		if (CanCastle(CASTLING_QUEENSIDE[active_color]))
			moves.push_back(Move(active_piece, Square(KING_SQUARE[active_color]), Square(ROOK_SQUARE_QUEENSIDE[active_color])).SetCastling());
	}
}

void MoveGenerator2::GenerateKnightMoves(std::vector<Move>& moves) const {
	Piece active_piece = Piece(Piece::TYPE_KNIGHT, active_color);
	BitBoard pieces_board = position.GetBitBoard(active_piece);
	BitBoard piece_board;

	Square from_square;
	while (pieces_board.ConsumeLowestSquare(from_square)) {
		//pinned?
		piece_board = BitBoard().Set(from_square.GetValue());
		if ((piece_board & pinned_board).NotEmpty())
			continue; //pinned knight can never move

		BitBoard to_board = MoveBoard::GetInstance().GetKnightMoves(from_square);
		to_board &= ~active_board; //exclude own pieces
		to_board &= block_board; //check evasion

		Square to_square;
		while (to_board.ConsumeLowestSquare(to_square)) {
			moves.push_back(Move(active_piece, from_square, to_square));
		}
	}
}

void MoveGenerator2::GeneratePawnMoves(std::vector<Move>& moves) const {
	static const uint8_t DOUBLE_PUSH_RANKS[2] = { 1Ui8, 6Ui8 };
	static const uint8_t PROMOTION_RANKS[2] = { 6Ui8, 1Ui8 };

	const Piece active_piece = Piece(Piece::TYPE_PAWN, active_color);

	BitBoard pieces_board = position.GetBitBoard(active_piece);
	BitBoard piece_board;
	BitBoard pin_ray_board;
	BitBoard to_board, to_board_2;
	Square from_square, to_square;
	BitBoard double_push_board;
	while (pieces_board.ConsumeLowestSquare(from_square)) {
		
		piece_board = BitBoard().Set(from_square.GetValue());
		if ((piece_board & pinned_board).NotEmpty()) {
			pin_ray_board = pin_rays.at(from_square.GetValue());
		}
		else {
			pin_ray_board = BitBoard(~0Ui64);
		}

		//single push
		to_board = MoveBoard::GetInstance().GetPawnPushes(from_square, active_color);
		to_board &= ~combined_board; //exclude all pieces: only silent moves
		to_board &= pin_ray_board; //pinned movement
		double_push_board = BitBoard(to_board); //must measure before check evasion
		to_board &= block_board; //check evasion

		if (to_board.GetLowestSquare(to_square)) {
			if (from_square.GetY() == PROMOTION_RANKS[active_color]) {
				moves.push_back(Move(active_piece, from_square, to_square, Piece(Piece::TYPE_QUEEN, active_color)));
				moves.push_back(Move(active_piece, from_square, to_square, Piece(Piece::TYPE_BISHOP, active_color)));
				moves.push_back(Move(active_piece, from_square, to_square, Piece(Piece::TYPE_ROOK, active_color)));
				moves.push_back(Move(active_piece, from_square, to_square, Piece(Piece::TYPE_KNIGHT, active_color)));
			}
			else {
				moves.push_back(Move(active_piece, from_square, to_square));
			}
		}

		//double push
		if (to_board.GetLowestSquare(to_square) && from_square.GetY() == DOUBLE_PUSH_RANKS[active_color]) {
			to_board_2 = MoveBoard::GetInstance().GetPawnPushes(to_square, active_color);
			to_board_2 &= ~combined_board; //exclude all pieces: only silent moves
			to_board_2 &= block_board; //check evasion
			to_board_2 &= pin_ray_board; //pinned movement

			if (to_board_2.GetLowestSquare(to_square)) {
				moves.push_back(Move(active_piece, from_square, to_square).SetDoublePush());
			}
		}


		//captures
		to_board = MoveBoard::GetInstance().GetPawnCaptures(from_square, active_color);
		BitBoard pawn_targets = BitBoard(inactive_board);
		BitBoard pawn_block_board = BitBoard(block_board);

		//add EP square to list of targets for pawn captures
		Square ep_square;
		bool has_ep_square = position.GetEpSquare(ep_square);
		if (has_ep_square) {
			BitBoard ep_board = BitBoard().Set(ep_square.GetValue());
			pawn_targets |= ep_board;
			//if enemy pawn is a checker, and is capturable by ep, add ep square to pawn_block_board
			if (active_color == Piece::COLOR_WHITE) {
				pawn_block_board |= (block_board & position.GetBitBoard(Piece(Piece::TYPE_PAWN, inactive_color))).Up() & ep_board;
			}
			else {
				pawn_block_board |= (block_board & position.GetBitBoard(Piece(Piece::TYPE_PAWN, inactive_color))).Down() & ep_board;
			}
		}

		to_board &= pawn_targets; //only captures
		to_board &= pawn_block_board; //check evasion
		to_board &= pin_ray_board; //pinned movement

		while (to_board.ConsumeLowestSquare(to_square)) {
			if (from_square.GetY() == PROMOTION_RANKS[active_color]) {
				moves.push_back(Move(active_piece, from_square, to_square, Piece(Piece::TYPE_QUEEN, active_color)));
				moves.push_back(Move(active_piece, from_square, to_square, Piece(Piece::TYPE_BISHOP, active_color)));
				moves.push_back(Move(active_piece, from_square, to_square, Piece(Piece::TYPE_ROOK, active_color)));
				moves.push_back(Move(active_piece, from_square, to_square, Piece(Piece::TYPE_KNIGHT, active_color)));
			}
			else {
				Move move = Move(active_piece, from_square, to_square);
				if (has_ep_square && to_square == ep_square) {
					//TODO double check discoverd check from ep capture
					move.SetEpCapture();
				}
				moves.push_back(move);
			}
		}
	}
}

void MoveGenerator2::GenerateRookMoves(std::vector<Move>& moves) const {
	const Piece piece = Piece(Piece::TYPE_ROOK, position.GetActiveColor());

	BitBoard pieces_board = position.GetBitBoard(piece);
	BitBoard piece_board;
	BitBoard pin_ray_board;
	Square piece_square;
	while (pieces_board.ConsumeLowestSquare(piece_square)) {
		
		piece_board = BitBoard().Set(piece_square.GetValue());
		if ((piece_board & pinned_board).NotEmpty()) {
			pin_ray_board = pin_rays.at(piece_square.GetValue());
		}
		else {
			pin_ray_board = BitBoard(~0Ui64);
		}

		GenerateRayMoves(piece_square, MoveBoard::DIR_UP, &BitBoard::GetLowestSquare, piece, pin_ray_board, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_RIGHT, &BitBoard::GetLowestSquare, piece, pin_ray_board, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_DOWN, &BitBoard::GetHighestSquare, piece, pin_ray_board, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_LEFT, &BitBoard::GetHighestSquare, piece, pin_ray_board, moves);
	}
}

void MoveGenerator2::GenerateBishopMoves(std::vector<Move>& moves) const {
	const Piece piece = Piece(Piece::TYPE_BISHOP, position.GetActiveColor());

	BitBoard pieces_board = position.GetBitBoard(piece);
	BitBoard piece_board;
	BitBoard pin_ray_board;
	Square piece_square;
	while (pieces_board.ConsumeLowestSquare(piece_square)) {
		piece_board = BitBoard().Set(piece_square.GetValue());
		if ((piece_board & pinned_board).NotEmpty()) {
			pin_ray_board = pin_rays.at(piece_square.GetValue());
		}
		else {
			pin_ray_board = BitBoard(~0Ui64);
		}

		GenerateRayMoves(piece_square, MoveBoard::DIR_UP_RIGHT, &BitBoard::GetLowestSquare, piece, pin_ray_board, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_DOWN_RIGHT, &BitBoard::GetHighestSquare, piece, pin_ray_board, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_DOWN_LEFT, &BitBoard::GetHighestSquare, piece, pin_ray_board, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_UP_LEFT, &BitBoard::GetLowestSquare, piece, pin_ray_board, moves);
	}
}

void MoveGenerator2::GenerateQueenMoves(std::vector<Move>& moves) const {
	const Piece piece = Piece(Piece::TYPE_QUEEN, position.GetActiveColor());

	BitBoard pieces_board = position.GetBitBoard(piece);
	BitBoard piece_board;
	BitBoard pin_ray_board;
	Square piece_square;
	while (pieces_board.ConsumeLowestSquare(piece_square)) {
		piece_board = BitBoard().Set(piece_square.GetValue());
		if ((piece_board & pinned_board).NotEmpty()) {
			pin_ray_board = pin_rays.at(piece_square.GetValue());
		}
		else {
			pin_ray_board = BitBoard(~0Ui64);
		}
		//rook moves
		GenerateRayMoves(piece_square, MoveBoard::DIR_UP, &BitBoard::GetLowestSquare, piece, pin_ray_board, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_RIGHT, &BitBoard::GetLowestSquare, piece, pin_ray_board, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_DOWN, &BitBoard::GetHighestSquare, piece, pin_ray_board, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_LEFT, &BitBoard::GetHighestSquare, piece, pin_ray_board, moves);
		//bishop moves
		GenerateRayMoves(piece_square, MoveBoard::DIR_UP_RIGHT, &BitBoard::GetLowestSquare, piece, pin_ray_board, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_DOWN_RIGHT, &BitBoard::GetHighestSquare, piece, pin_ray_board, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_DOWN_LEFT, &BitBoard::GetHighestSquare, piece, pin_ray_board, moves);
		GenerateRayMoves(piece_square, MoveBoard::DIR_UP_LEFT, &BitBoard::GetLowestSquare, piece, pin_ray_board, moves);
	}
}

bool MoveGenerator2::IsCheck(uint8_t color) {
	BitBoard king_board = position.GetBitBoard(Piece(Piece::TYPE_KING, color));
	Square square;
	if (king_board.GetLowestSquare(square)) {
		return IsSquareAttacked(square, color ^ 1Ui8);
	}

	return false;
}


bool MoveGenerator2::SetMoveFlags(Move& move) {
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

uint8_t MoveGenerator2::GetCheckerCount() {
	return checker_count;
}

BitBoard MoveGenerator2::GetBlockBoard() {
	return block_board;
}

void MoveGenerator2::GenerateRayMoves(const Square& from_square, const uint8_t dir, bool(BitBoard::*find_nearest_square)(Square&) const, const Piece active_piece, const BitBoard pin_ray_board, std::vector<Move>& moves) const {
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

	move_board &= block_board; //check evasion
	move_board &= pin_ray_board; //pinned movement

	Square to_square;
	while (move_board.ConsumeLowestSquare(to_square)) {
		moves.push_back(Move(active_piece, from_square, to_square));
	}
}

void MoveGenerator2::GenerateCheckInfo() {
	block_board = BitBoard(0);

	Square square;
	if (!position.GetPieceSquare(Piece(Piece::TYPE_KING, active_color), square))
		return;

	//attacked by pawn
	//pretend we are a pawn, can we capture opponent's pawn?
	BitBoard pawn_capture_board = MoveBoard::GetInstance().GetPawnCaptures(square, active_color);
	BitBoard attacking_pawn_board = position.GetBitBoard(Piece(Piece::TYPE_PAWN, inactive_color));
	BitBoard pawn_checker_board = pawn_capture_board & attacking_pawn_board;
	checker_count += pawn_checker_board.NotEmpty();
	block_board |= pawn_checker_board;

	//attacked by knight
	//pretend we are a knight, can we capture opponent's knight?
	BitBoard knight_capture_board = MoveBoard::GetInstance().GetKnightMoves(square);
	BitBoard attacking_knight_board = position.GetBitBoard(Piece(Piece::TYPE_KNIGHT, inactive_color));
	BitBoard knight_checker_board = knight_capture_board & attacking_knight_board;
	checker_count += knight_checker_board.NotEmpty();
	block_board |= knight_checker_board;

	//note: enemy king cannot give check

	//attacked by sliding pieces?
	BitBoard check_ray_board;

	check_ray_board = GetCheckRayBoard(square, MoveBoard::DIR_UP, &BitBoard::GetLowestSquare, Piece::TYPE_ROOK);
	checker_count += check_ray_board.NotEmpty();
	block_board |= check_ray_board;

	check_ray_board = GetCheckRayBoard(square, MoveBoard::DIR_DOWN, &BitBoard::GetHighestSquare, Piece::TYPE_ROOK);
	checker_count += check_ray_board.NotEmpty();
	block_board |= check_ray_board;

	check_ray_board = GetCheckRayBoard(square, MoveBoard::DIR_RIGHT, &BitBoard::GetLowestSquare, Piece::TYPE_ROOK);
	checker_count += check_ray_board.NotEmpty();
	block_board |= check_ray_board;

	check_ray_board = GetCheckRayBoard(square, MoveBoard::DIR_LEFT, &BitBoard::GetHighestSquare, Piece::TYPE_ROOK);
	checker_count += check_ray_board.NotEmpty();
	block_board |= check_ray_board;

	check_ray_board = GetCheckRayBoard(square, MoveBoard::DIR_UP_RIGHT, &BitBoard::GetLowestSquare, Piece::TYPE_BISHOP);
	checker_count += check_ray_board.NotEmpty();
	block_board |= check_ray_board;

	check_ray_board = GetCheckRayBoard(square, MoveBoard::DIR_DOWN_LEFT, &BitBoard::GetHighestSquare, Piece::TYPE_BISHOP);
	checker_count += check_ray_board.NotEmpty();
	block_board |= check_ray_board;

	check_ray_board = GetCheckRayBoard(square, MoveBoard::DIR_UP_LEFT, &BitBoard::GetLowestSquare, Piece::TYPE_BISHOP);
	checker_count += check_ray_board.NotEmpty();
	block_board |= check_ray_board;

	check_ray_board = GetCheckRayBoard(square, MoveBoard::DIR_DOWN_RIGHT, &BitBoard::GetHighestSquare, Piece::TYPE_BISHOP);
	checker_count += check_ray_board.NotEmpty();
	block_board |= check_ray_board;

	if (block_board.Empty())
		block_board = BitBoard(0xFFFFFFFFFFFFFFFFUi64);
}

bool MoveGenerator2::IsSquareAttacked(const Square& square, const uint8_t attacking_color) const {
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

BitBoard MoveGenerator2::GetCheckRayBoard(const Square king_square, const uint8_t dir, bool(BitBoard::*find_nearest_square)(Square&) const, const uint8_t rook_or_bishop_type) const {
	BitBoard forward_ray_board = MoveBoard::GetInstance().GetRay(king_square.GetValue(), dir);
	BitBoard forward_ray_intersect = forward_ray_board & combined_board;

	if (forward_ray_intersect.Empty())
		return BitBoard();

	Square nearest_square;
	(forward_ray_intersect.*find_nearest_square)(nearest_square);
	BitBoard nearest_square_board = BitBoard().Set(nearest_square.GetValue());
	BitBoard attacker_board = position.GetBitBoard(Piece(Piece::TYPE_QUEEN, inactive_color)) | position.GetBitBoard(Piece(rook_or_bishop_type, inactive_color));
	if ((nearest_square_board & attacker_board).NotEmpty()) {
		forward_ray_board ^= MoveBoard::GetInstance().GetRay(nearest_square.GetValue(), dir); //flip bits of "shadow ray"
		return forward_ray_board;
	}

	return BitBoard();
}

bool MoveGenerator2::IsAttackedFromDirection(const Square square, const uint8_t dir, bool(BitBoard::*find_nearest_square)(Square&) const, const uint8_t rook_or_bishop_type, const uint8_t attacking_color) const {
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

bool MoveGenerator2::CanCastle(const int castling_index) const {
	//check castling rights
	if (!position.GetCastlingStatus(castling_index))
		return false;

	//squares between king and rook must be empty
	if ((MoveBoard::GetInstance().GetCastlingEmptySquares(castling_index) & combined_board).NotEmpty())
		return false;

	//squares king passes through (including current position) may not be attacked by opponent
	return (MoveBoard::GetInstance().GetCastlingSafeSquares(castling_index) & danger_board).Empty();
}

void MoveGenerator2::GenerateDangerBoard() {
	Square piece_square;
	BitBoard piece_board;

	BitBoard king_board = position.GetBitBoard(Piece(Piece::TYPE_KING, active_color));

	//pawn captures (generate all at once)
	BitBoard pawn_pos_board = position.GetBitBoard(Piece(Piece::TYPE_PAWN, inactive_color));
	BitBoard pawn_capture_board = pawn_pos_board.Clone().Up().Left() | pawn_pos_board.Clone().Up().Right();
	danger_board |= pawn_capture_board;

	//king
	if (position.GetPieceSquare(Piece(Piece::TYPE_KING, inactive_color), piece_square)) {
		danger_board |= MoveBoard::GetInstance().GetKingMoves(piece_square);
	}

	//knight
	piece_board = position.GetBitBoard(Piece(Piece::TYPE_KNIGHT, inactive_color));
	while (piece_board.ConsumeLowestSquare(piece_square)) {
		danger_board |= MoveBoard::GetInstance().GetKnightMoves(piece_square);
	}

	Piece piece;
	//rook
	piece_board = position.GetBitBoard(Piece(Piece::TYPE_ROOK, inactive_color));
	while (piece_board.ConsumeLowestSquare(piece_square)) {
		//rook moves
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_UP, &BitBoard::GetLowestSquare, king_board);
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_RIGHT, &BitBoard::GetLowestSquare, king_board);
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_DOWN, &BitBoard::GetHighestSquare, king_board);
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_LEFT, &BitBoard::GetHighestSquare, king_board);
	}

	//bishop
	piece_board = position.GetBitBoard(Piece(Piece::TYPE_BISHOP, inactive_color));
	while (piece_board.ConsumeLowestSquare(piece_square)) {
		//bishop moves
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_UP_RIGHT, &BitBoard::GetLowestSquare, king_board);
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_DOWN_RIGHT, &BitBoard::GetHighestSquare, king_board);
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_DOWN_LEFT, &BitBoard::GetHighestSquare, king_board);
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_UP_LEFT, &BitBoard::GetLowestSquare, king_board);
	}

	//queen
	piece_board = position.GetBitBoard(Piece(Piece::TYPE_QUEEN, inactive_color));
	while (piece_board.ConsumeLowestSquare(piece_square)) {
		//rook moves
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_UP, &BitBoard::GetLowestSquare, king_board);
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_RIGHT, &BitBoard::GetLowestSquare, king_board);
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_DOWN, &BitBoard::GetHighestSquare, king_board);
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_LEFT, &BitBoard::GetHighestSquare, king_board);
		//bishop moves
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_UP_RIGHT, &BitBoard::GetLowestSquare, king_board);
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_DOWN_RIGHT, &BitBoard::GetHighestSquare, king_board);
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_DOWN_LEFT, &BitBoard::GetHighestSquare, king_board);
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_UP_LEFT, &BitBoard::GetLowestSquare, king_board);
	}
}

BitBoard MoveGenerator2::GenerateDangerRayBoard(const Square from_square, const uint8_t dir, bool(BitBoard::*find_nearest_square)(Square&) const, const BitBoard exclude_board) const {
	BitBoard move_board = MoveBoard::GetInstance().GetRay(from_square, dir);
	BitBoard intersect_board = move_board & combined_board & ~exclude_board; //substract own king when finding danger squares

	if (intersect_board.NotEmpty()) {
		Square nearest_square;
		(intersect_board.*find_nearest_square)(nearest_square);
		move_board ^= MoveBoard::GetInstance().GetRay(nearest_square, dir);
	}

	return move_board;
}

void MoveGenerator2::GeneratePinInfo() {

	pinned_board = BitBoard(0);
	pin_rays.clear();

	BitBoard king_board = position.GetBitBoard(Piece(Piece::TYPE_KING, active_color));
	Square king_square;
	king_board.GetLowestSquare(king_square);

	//rook directions
	GeneratePinRayInfo(king_square, MoveBoard::DIR_UP, &BitBoard::GetLowestSquare, Piece::TYPE_ROOK);
	GeneratePinRayInfo(king_square, MoveBoard::DIR_RIGHT, &BitBoard::GetLowestSquare, Piece::TYPE_ROOK);
	GeneratePinRayInfo(king_square, MoveBoard::DIR_DOWN, &BitBoard::GetHighestSquare, Piece::TYPE_ROOK);
	GeneratePinRayInfo(king_square, MoveBoard::DIR_LEFT, &BitBoard::GetHighestSquare, Piece::TYPE_ROOK);
	//bishop directions
	GeneratePinRayInfo(king_square, MoveBoard::DIR_UP_RIGHT, &BitBoard::GetLowestSquare, Piece::TYPE_BISHOP);
	GeneratePinRayInfo(king_square, MoveBoard::DIR_DOWN_RIGHT, &BitBoard::GetHighestSquare, Piece::TYPE_BISHOP);
	GeneratePinRayInfo(king_square, MoveBoard::DIR_DOWN_LEFT, &BitBoard::GetHighestSquare, Piece::TYPE_BISHOP);
	GeneratePinRayInfo(king_square, MoveBoard::DIR_UP_LEFT, &BitBoard::GetLowestSquare, Piece::TYPE_BISHOP);
}

void MoveGenerator2::GeneratePinRayInfo(const Square king_square, const uint8_t dir, bool(BitBoard::* find_nearest_square)(Square&) const, const uint8_t rook_or_bishop_type) {

	//generate rays from our king
	//exclude our own pieces
	//if we hit a (relevant) slider, we have a potential pin
	//calculate the ray from king to enemy slider including the enemy slider
	//intersect with own pieces
	//if the popcount of the intersection is equal to one, we have a pinned piece that can only move on the ray

	//store pinners in a pinners-board
	//store squares and ray-board in a map

	BitBoard ray_board = MoveBoard::GetInstance().GetRay(king_square.GetValue(), dir);
	BitBoard inter_board = ray_board & inactive_board;
	Square nearest_square;
	if ((inter_board.*find_nearest_square)(nearest_square)) {
		BitBoard nearest_board = BitBoard().Set(nearest_square.GetValue());
		BitBoard slider_board = position.GetBitBoard(Piece(Piece::TYPE_QUEEN, inactive_color)) | position.GetBitBoard(Piece(rook_or_bishop_type, inactive_color));
		if ((nearest_board & slider_board).NotEmpty()) {
			ray_board ^= MoveBoard::GetInstance().GetRay(nearest_square.GetValue(), dir);
			BitBoard potential_pinner_board = ray_board & active_board;
			if (potential_pinner_board.PopCount() == 1Ui8) {
				//exactly one active piece in between
				pinned_board |= potential_pinner_board;
				Square pinner_square;
				potential_pinner_board.GetLowestSquare(pinner_square);
				pin_rays.insert(pair<uint8_t, BitBoard>(pinner_square.GetValue(), ray_board));
			}
		}
	}
}
