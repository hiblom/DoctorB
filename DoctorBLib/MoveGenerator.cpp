#include "stdafx.h"
#include "MoveGenerator.h"
#include <array>
#include "Square.h"
#include "MoveBoard.h"
#include "Constants.h"

using namespace std;

MoveGenerator::MoveGenerator(const Position& position) : position_(position), only_captures_(false) {
	Initialize();
	captures_board = BitBoard(~0Ui64); //only captures = false, so all squares are available to move to
}

MoveGenerator::MoveGenerator(const Position & position, const bool only_captures): position_(position), only_captures_(only_captures) {
	Initialize();
	if (only_captures) {
		captures_board = inactive_board; //we can only move to squres with opponent's piece
	}
	else
		captures_board = BitBoard(~0Ui64); //only captures = false, so all squares are available to move to
}

MoveGenerator::~MoveGenerator() {
}

void MoveGenerator::Initialize() {
	active_color = position_.GetActiveColor();
	inactive_color = position_.GetActiveColor() ^ 1Ui8;
	checker_count = 0;

	//previous loop unrolled to lessen the number of loops
	active_board = position_.GetBitBoard(Piece(Piece::TYPE_PAWN, active_color));
	inactive_board = position_.GetBitBoard(Piece(Piece::TYPE_PAWN, inactive_color));

	active_board |= position_.GetBitBoard(Piece(Piece::TYPE_KING, active_color));
	inactive_board |= position_.GetBitBoard(Piece(Piece::TYPE_KING, inactive_color));

	active_board |= position_.GetBitBoard(Piece(Piece::TYPE_QUEEN, active_color));
	inactive_board |= position_.GetBitBoard(Piece(Piece::TYPE_QUEEN, inactive_color));

	active_board |= position_.GetBitBoard(Piece(Piece::TYPE_ROOK, active_color));
	inactive_board |= position_.GetBitBoard(Piece(Piece::TYPE_ROOK, inactive_color));

	active_board |= position_.GetBitBoard(Piece(Piece::TYPE_BISHOP, active_color));
	inactive_board |= position_.GetBitBoard(Piece(Piece::TYPE_BISHOP, inactive_color));

	active_board |= position_.GetBitBoard(Piece(Piece::TYPE_KNIGHT, active_color));
	inactive_board |= position_.GetBitBoard(Piece(Piece::TYPE_KNIGHT, inactive_color));

	combined_board = active_board | inactive_board;

	//some initialization
	GenerateDangerBoard(); //mark the squares that are not accessible for the king
	GenerateCheckInfo(); //count the number of checkers and generate block board
	GeneratePinInfo(); //mark pinned pieces and pin rays along which they can move
}

void MoveGenerator::GenerateMoves(vector<Move>& moves) {
	GenerateKingMoves(moves);

	if (checker_count >= 2)
		return;

	GeneratePawnMoves(moves);
	GenerateQueenMoves(moves);
	GenerateRookMoves(moves);
	GenerateBishopMoves(moves);
	GenerateKnightMoves(moves);
}

void MoveGenerator::GenerateKingMoves(vector<Move>& moves) const {
	static const array<int, 2> CASTLING_KINGSIDE { { Constants::CASTLING_BLACK_KINGSIDE, Constants::CASTLING_WHITE_KINGSIDE} };
	static const array<int, 2> CASTLING_QUEENSIDE { { Constants::CASTLING_BLACK_QUEENSIDE, Constants::CASTLING_WHITE_QUEENSIDE } };
	static const array<uint8_t, 2> KING_SQUARE { { Square::E8, Square::E1 } };
	static const array<uint8_t, 2> ROOK_SQUARE_KINGSIDE { { Square::G8, Square::G1 } };
	static const array<uint8_t, 2> ROOK_SQUARE_QUEENSIDE { { Square::C8, Square::C1 } };

	Piece active_piece = Piece(Piece::TYPE_KING, active_color);
	Square from_square;

	if (!position_.GetPieceSquare(active_piece, from_square))
		return;

	BitBoard to_board = MoveBoard::GetInstance().GetKingMoves(from_square);
	to_board &= ~active_board; //exclude own pieces
	to_board &= ~danger_board; //exclude danger board
	to_board &= captures_board; //captures only - mode

	Square to_square;
	while (to_board.PopLowestSquare(to_square)) {
		moves.push_back(Move(active_piece, from_square, to_square).SetCapture(IsCapture(to_square)));
	}

	//castling
	if (checker_count == 0 && !only_captures_) {
		if (CanCastle(CASTLING_KINGSIDE[active_color]))
			moves.push_back(Move(active_piece, Square(KING_SQUARE[active_color]), Square(ROOK_SQUARE_KINGSIDE[active_color])).SetCastling());

		if (CanCastle(CASTLING_QUEENSIDE[active_color]))
			moves.push_back(Move(active_piece, Square(KING_SQUARE[active_color]), Square(ROOK_SQUARE_QUEENSIDE[active_color])).SetCastling());
	}
}

void MoveGenerator::GenerateKnightMoves(vector<Move>& moves) const {
	Piece active_piece = Piece(Piece::TYPE_KNIGHT, active_color);
	BitBoard pieces_board = position_.GetBitBoard(active_piece);
	BitBoard piece_board;

	Square from_square;
	while (pieces_board.PopLowestSquare(from_square)) {
		//pinned?
		piece_board = BitBoard().Set(from_square.GetValue());
		if ((piece_board & pinned_board).NotEmpty())
			continue; //pinned knight can never move

		BitBoard to_board = MoveBoard::GetInstance().GetKnightMoves(from_square);
		to_board &= ~active_board; //exclude own pieces
		to_board &= block_board; //check evasion
		to_board &= captures_board; //captures only - mode

		Square to_square;
		while (to_board.PopLowestSquare(to_square)) {
			moves.push_back(Move(active_piece, from_square, to_square).SetCapture(IsCapture(to_square)));
		}
	}
}

void MoveGenerator::GeneratePawnMoves(std::vector<Move>& moves) const {
	static const array<uint8_t, 2> DOUBLE_PUSH_RANKS { { 6Ui8, 1Ui8 } };
	static const array<uint8_t, 2> PROMOTION_RANKS{ { 1Ui8, 6Ui8 } };
	BitBoard&(BitBoard::*pawn_forward)() = MoveBoard::GetInstance().Forward[active_color];

	const Piece active_piece = Piece(Piece::TYPE_PAWN, active_color);

	BitBoard pieces_board = position_.GetBitBoard(active_piece);
	BitBoard piece_board;
	BitBoard pin_ray_board;
	BitBoard to_board, to_board_2;
	Square from_square, to_square;
	BitBoard double_push_board;
	while (pieces_board.PopLowestSquare(from_square)) {
		
		piece_board = BitBoard().Set(from_square.GetValue());
		if ((piece_board & pinned_board).NotEmpty()) {
			pin_ray_board = pin_rays.at(from_square.GetValue());
		}
		else {
			pin_ray_board = BitBoard(~0Ui64);
		}

		if (!only_captures_) {
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
			if (double_push_board.GetLowestSquare(to_square) && from_square.GetY() == DOUBLE_PUSH_RANKS[active_color]) {
				to_board_2 = MoveBoard::GetInstance().GetPawnPushes(to_square, active_color);
				to_board_2 &= ~combined_board; //exclude all pieces: only silent moves
				to_board_2 &= block_board; //check evasion
				to_board_2 &= pin_ray_board; //pinned movement

				if (to_board_2.GetLowestSquare(to_square)) {
					moves.push_back(Move(active_piece, from_square, to_square).SetDoublePush());
				}
			}
		}


		//captures
		to_board = MoveBoard::GetInstance().GetPawnCaptures(from_square, active_color);
		BitBoard pawn_targets = BitBoard(inactive_board);
		BitBoard pawn_block_board = BitBoard(block_board);

		//add EP square to list of targets for pawn captures
		Square ep_square;
		bool has_ep_square = position_.GetEpSquare(ep_square);
		if (has_ep_square) {
			BitBoard ep_board = BitBoard().Set(ep_square.GetValue());
			pawn_targets |= ep_board;
			//if enemy pawn is a checker, and is capturable by ep, add ep square to pawn_block_board
			BitBoard checking_pawns = block_board & position_.GetBitBoard(Piece(Piece::TYPE_PAWN, inactive_color));
			pawn_block_board |= (checking_pawns.*pawn_forward)() & ep_board;
		}

		to_board &= pawn_targets; //only captures
		to_board &= pawn_block_board; //check evasion
		to_board &= pin_ray_board; //pinned movement

		while (to_board.PopLowestSquare(to_square)) {
			if (from_square.GetY() == PROMOTION_RANKS[active_color]) {
				moves.push_back(Move(active_piece, from_square, to_square, Piece(Piece::TYPE_QUEEN, active_color)).SetCapture(true));
				moves.push_back(Move(active_piece, from_square, to_square, Piece(Piece::TYPE_BISHOP, active_color)).SetCapture(true));
				moves.push_back(Move(active_piece, from_square, to_square, Piece(Piece::TYPE_ROOK, active_color)).SetCapture(true));
				moves.push_back(Move(active_piece, from_square, to_square, Piece(Piece::TYPE_KNIGHT, active_color)).SetCapture(true));
			}
			else {
				Move move;
				if (has_ep_square && to_square == ep_square) {
					if (CheckEpDiscoveredCheck(ep_square, from_square))
						continue;

					move = Move(active_piece, from_square, to_square).SetEpCapture();
				}
				else {
					move = Move(active_piece, from_square, to_square).SetCapture(true);
				}
				moves.push_back(move);
			}
		}
	}
}

void MoveGenerator::GenerateRookMoves(std::vector<Move>& moves) const {
	const Piece piece = Piece(Piece::TYPE_ROOK, position_.GetActiveColor());

	BitBoard pieces_board = position_.GetBitBoard(piece);
	BitBoard piece_board;
	BitBoard pin_ray_board;
	Square piece_square;
	while (pieces_board.PopLowestSquare(piece_square)) {
		
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

void MoveGenerator::GenerateBishopMoves(std::vector<Move>& moves) const {
	const Piece piece = Piece(Piece::TYPE_BISHOP, position_.GetActiveColor());

	BitBoard pieces_board = position_.GetBitBoard(piece);
	BitBoard piece_board;
	BitBoard pin_ray_board;
	Square piece_square;
	while (pieces_board.PopLowestSquare(piece_square)) {
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

void MoveGenerator::GenerateQueenMoves(std::vector<Move>& moves) const {
	const Piece piece = Piece(Piece::TYPE_QUEEN, position_.GetActiveColor());

	BitBoard pieces_board = position_.GetBitBoard(piece);
	BitBoard piece_board;
	BitBoard pin_ray_board;
	Square piece_square;
	while (pieces_board.PopLowestSquare(piece_square)) {
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

bool MoveGenerator::IsCheck() {
	return checker_count > 0;
}

bool MoveGenerator::SetMoveFlags(Move& move) {
	Piece piece;
	if (!position_.GetPiece(move.GetSquareFrom(), piece))
		return false;

	//piece
	move.SetPiece(piece);

	//capture
	Piece captured_piece;
	move.SetCapture(position_.GetPiece(move.GetSquareTo(), captured_piece));

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
		if (position_.GetEpSquare(ep_square)) {
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

uint8_t MoveGenerator::GetCheckerCount() {
	return checker_count;
}

BitBoard MoveGenerator::GetBlockBoard() {
	return block_board;
}

//get capture by least valuable attacker by color on a certain square
bool MoveGenerator::GetLvaCapture(const Square to_square, Move& move) const {
	vector<Move> moves(16);

	//pawn
	moves.clear();
	GeneratePawnMoves(moves);
	for (Move piece_move : moves) {
		if (piece_move.GetSquareTo() == to_square) {
			move = piece_move;
			return true;
		}
	}

	//knight
	moves.clear();
	GenerateKnightMoves(moves);
	for (Move piece_move : moves) {
		if (piece_move.GetSquareTo() == to_square) {
			move = piece_move;
			return true;
		}
	}

	//bishop
	moves.clear();
	GenerateBishopMoves(moves);
	for (Move piece_move : moves) {
		if (piece_move.GetSquareTo() == to_square) {
			move = piece_move;
			return true;
		}
	}

	//rook
	moves.clear();
	GenerateRookMoves(moves);
	for (Move piece_move : moves) {
		if (piece_move.GetSquareTo() == to_square) {
			move = piece_move;
			return true;
		}
	}

	//queen
	moves.clear();
	GenerateQueenMoves(moves);
	for (Move piece_move : moves) {
		if (piece_move.GetSquareTo() == to_square) {
			move = piece_move;
			return true;
		}
	}

	//king
	moves.clear();
	GenerateKingMoves(moves);
	for (Move piece_move : moves) {
		if (piece_move.GetSquareTo() == to_square) {
			move = piece_move;
			return true;
		}
	}

	return false;
}

void MoveGenerator::GenerateRayMoves(const Square& from_square, const uint8_t dir, bool(BitBoard::*find_nearest_square)(Square&) const, const Piece active_piece, const BitBoard pin_ray_board, std::vector<Move>& moves) const {
	//BitBoard move_board;
	BitBoard move_board = MoveBoard::GetInstance().GetRay(from_square, dir);
	BitBoard intersect_board = move_board & combined_board;

	if (intersect_board.NotEmpty()) {
		Square nearest_square;
		(intersect_board.*find_nearest_square)(nearest_square);

		if (only_captures_) {
			//shortcut when only captures, only look at intersect square
			move_board = BitBoard().Set(nearest_square.GetValue());
			move_board &= captures_board; //only captures
			move_board &= block_board; //check evasion
			move_board &= pin_ray_board; //pinned movement
			if (move_board.NotEmpty()) 
				moves.push_back(Move(active_piece, from_square, nearest_square).SetCapture(true));
			return;
		}

		//slight change after reading https://www.chessprogramming.org/Classical_Approach
		move_board ^= MoveBoard::GetInstance().GetRay(nearest_square, dir);
		move_board &= ~active_board; //subtract own pieces
	}
	
	if (only_captures_)
		return;

	move_board &= block_board; //check evasion
	move_board &= pin_ray_board; //pinned movement

	Square to_square;
	while (move_board.PopLowestSquare(to_square)) {
		moves.push_back(Move(active_piece, from_square, to_square).SetCapture(IsCapture(to_square)));
	}
}

void MoveGenerator::GenerateCheckInfo() {
	block_board = BitBoard(0);

	Square square;
	if (!position_.GetPieceSquare(Piece(Piece::TYPE_KING, active_color), square))
		return;

	//attacked by pawn
	//pretend we are a pawn, can we capture opponent's pawn?
	BitBoard pawn_capture_board = MoveBoard::GetInstance().GetPawnCaptures(square, active_color);
	BitBoard attacking_pawn_board = position_.GetBitBoard(Piece(Piece::TYPE_PAWN, inactive_color));
	BitBoard pawn_checker_board = pawn_capture_board & attacking_pawn_board;
	checker_count += pawn_checker_board.NotEmpty();
	block_board |= pawn_checker_board;

	//attacked by knight
	//pretend we are a knight, can we capture opponent's knight?
	BitBoard knight_capture_board = MoveBoard::GetInstance().GetKnightMoves(square);
	BitBoard attacking_knight_board = position_.GetBitBoard(Piece(Piece::TYPE_KNIGHT, inactive_color));
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

BitBoard MoveGenerator::GetCheckRayBoard(const Square king_square, const uint8_t dir, bool(BitBoard::*find_nearest_square)(Square&) const, const uint8_t rook_or_bishop_type) const {
	BitBoard forward_ray_board = MoveBoard::GetInstance().GetRay(king_square.GetValue(), dir);
	BitBoard forward_ray_intersect = forward_ray_board & combined_board;

	if (forward_ray_intersect.Empty())
		return BitBoard();

	Square nearest_square;
	(forward_ray_intersect.*find_nearest_square)(nearest_square);
	BitBoard nearest_square_board = BitBoard().Set(nearest_square.GetValue());
	BitBoard attacker_board = position_.GetBitBoard(Piece(Piece::TYPE_QUEEN, inactive_color)) | position_.GetBitBoard(Piece(rook_or_bishop_type, inactive_color));
	if ((nearest_square_board & attacker_board).NotEmpty()) {
		forward_ray_board ^= MoveBoard::GetInstance().GetRay(nearest_square.GetValue(), dir); //flip bits of "shadow ray"
		return forward_ray_board;
	}

	return BitBoard();
}

bool MoveGenerator::CanCastle(const int castling_index) const {
	//check castling rights
	if (!position_.GetCastlingStatus(castling_index))
		return false;

	//squares between king and rook must be empty
	if ((MoveBoard::GetInstance().GetCastlingEmptySquares(castling_index) & combined_board).NotEmpty())
		return false;

	//squares king passes through (including current position) may not be attacked by opponent
	return (MoveBoard::GetInstance().GetCastlingSafeSquares(castling_index) & danger_board).Empty();
}

void MoveGenerator::GenerateDangerBoard() {
	BitBoard&(BitBoard::*pawn_forward)() = MoveBoard::GetInstance().Forward[inactive_color];

	Square piece_square;
	BitBoard piece_board;

	BitBoard king_board = position_.GetBitBoard(Piece(Piece::TYPE_KING, active_color));

	//pawn captures (generate all at once)
	BitBoard pawn_pos_board = position_.GetBitBoard(Piece(Piece::TYPE_PAWN, inactive_color));
	BitBoard pawn_capture_board = (pawn_pos_board.Clone().*pawn_forward)().Left() | (pawn_pos_board.Clone().*pawn_forward)().Right();
	danger_board |= pawn_capture_board;

	//king
	if (position_.GetPieceSquare(Piece(Piece::TYPE_KING, inactive_color), piece_square)) {
		danger_board |= MoveBoard::GetInstance().GetKingMoves(piece_square);
	}

	//knight
	piece_board = position_.GetBitBoard(Piece(Piece::TYPE_KNIGHT, inactive_color));
	while (piece_board.PopLowestSquare(piece_square)) {
		danger_board |= MoveBoard::GetInstance().GetKnightMoves(piece_square);
	}

	Piece piece;
	//rook
	piece_board = position_.GetBitBoard(Piece(Piece::TYPE_ROOK, inactive_color));
	while (piece_board.PopLowestSquare(piece_square)) {
		//rook moves
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_UP, &BitBoard::GetLowestSquare, king_board);
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_RIGHT, &BitBoard::GetLowestSquare, king_board);
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_DOWN, &BitBoard::GetHighestSquare, king_board);
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_LEFT, &BitBoard::GetHighestSquare, king_board);
	}

	//bishop
	piece_board = position_.GetBitBoard(Piece(Piece::TYPE_BISHOP, inactive_color));
	while (piece_board.PopLowestSquare(piece_square)) {
		//bishop moves
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_UP_RIGHT, &BitBoard::GetLowestSquare, king_board);
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_DOWN_RIGHT, &BitBoard::GetHighestSquare, king_board);
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_DOWN_LEFT, &BitBoard::GetHighestSquare, king_board);
		danger_board |= GenerateDangerRayBoard(piece_square, MoveBoard::DIR_UP_LEFT, &BitBoard::GetLowestSquare, king_board);
	}

	//queen
	piece_board = position_.GetBitBoard(Piece(Piece::TYPE_QUEEN, inactive_color));
	while (piece_board.PopLowestSquare(piece_square)) {
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

BitBoard MoveGenerator::GenerateDangerRayBoard(const Square from_square, const uint8_t dir, bool(BitBoard::*find_nearest_square)(Square&) const, const BitBoard exclude_board) const {
	BitBoard move_board = MoveBoard::GetInstance().GetRay(from_square, dir);
	BitBoard intersect_board = move_board & combined_board & ~exclude_board; //substract own king when finding danger squares

	if (intersect_board.NotEmpty()) {
		Square nearest_square;
		(intersect_board.*find_nearest_square)(nearest_square);
		move_board ^= MoveBoard::GetInstance().GetRay(nearest_square, dir);
	}

	return move_board;
}

void MoveGenerator::GeneratePinInfo() {
	pinned_board = BitBoard(0Ui64);
	pin_rays.clear();

	BitBoard king_board = position_.GetBitBoard(Piece(Piece::TYPE_KING, active_color));
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

void MoveGenerator::GeneratePinRayInfo(const Square king_square, const uint8_t dir, bool(BitBoard::* find_nearest_square)(Square&) const, const uint8_t rook_or_bishop_type) {
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
		BitBoard slider_board = position_.GetBitBoard(Piece(Piece::TYPE_QUEEN, inactive_color)) | position_.GetBitBoard(Piece(rook_or_bishop_type, inactive_color));
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

bool MoveGenerator::CheckEpDiscoveredCheck(const Square ep_square, const Square capturing_square) const {
	//we only need to check horizontal discovered checks
	//the capturing pawn will be flagged as pinned piece when the discovered check is diagonal or vertical
	
	//active king on same rank as pawns?
	Square king_square;
	if (!position_.GetBitBoard(Piece(Piece::TYPE_KING, active_color)).GetLowestSquare(king_square))
		return false;

	if (king_square.GetY() != capturing_square.GetY())
		return false;

	//when taking away capturing pawn and captured pawn, is there a direct ray from an enemy slider to our king ?
	Square captured_square = Square(ep_square.GetX(), capturing_square.GetY());

	BitBoard after_ep_board = combined_board & ~BitBoard().Set(captured_square.GetValue()) & ~BitBoard().Set(capturing_square.GetValue());
	BitBoard potential_checkers = position_.GetBitBoard(Piece(Piece::TYPE_QUEEN, inactive_color)) | position_.GetBitBoard(Piece(Piece::TYPE_ROOK, inactive_color));
	
	//check attack from the right
	BitBoard ray_board = MoveBoard::GetInstance().GetRay(king_square, MoveBoard::DIR_RIGHT);
	BitBoard intersect_board = ray_board & after_ep_board;
	Square nearest_square;
	if (intersect_board.GetLowestSquare(nearest_square)) {
		if ((BitBoard().Set(nearest_square.GetValue()) & potential_checkers).NotEmpty())
			return true;
	}

	//check attack from the left
	ray_board = MoveBoard::GetInstance().GetRay(king_square, MoveBoard::DIR_LEFT);
	intersect_board = ray_board & after_ep_board;
	if (intersect_board.GetHighestSquare(nearest_square)) {
		if ((BitBoard().Set(nearest_square.GetValue()) & potential_checkers).NotEmpty())
			return true;
	}

	return false;
}

bool MoveGenerator::IsCapture(const Square to_square) const {
	return (BitBoard().Set(to_square.GetValue()) & inactive_board).NotEmpty();
}