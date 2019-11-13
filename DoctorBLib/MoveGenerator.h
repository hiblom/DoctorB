#pragma once
#include <vector>
#include <map>
#include "Position.h"
#include "Move.h"

class MoveGenerator {
public:
	MoveGenerator(const Position& position);
	MoveGenerator(const Position& position, const bool only_captures);
	~MoveGenerator();
	void GenerateMoves(std::vector<Move>& moves);
	void GenerateKingMoves(std::vector<Move>& moves) const;
	void GenerateKnightMoves(std::vector<Move>& moves) const;
	void GeneratePawnMoves(std::vector<Move>& moves) const;
	void GenerateRookMoves(std::vector<Move>& moves) const;
	void GenerateBishopMoves(std::vector<Move>& moves) const;
	void GenerateQueenMoves(std::vector<Move>& moves) const;
	void GenerateDangerBoard();
	bool IsCheck(uint8_t color);
	bool SetMoveFlags(Move& move);
	void GenerateCheckInfo();
	uint8_t GetCheckerCount();
	BitBoard GetBlockBoard();
	bool GetLvaCapture(const Square to_square, Move& move) const;
private:
	const Position& position_;
	const bool only_captures_;
	BitBoard active_board;
	BitBoard inactive_board;
	BitBoard combined_board;
	BitBoard danger_board;
	uint8_t active_color;
	uint8_t inactive_color;
	uint8_t checker_count;
	BitBoard block_board;
	BitBoard pinned_board;
	BitBoard captures_board;
	std::map<uint8_t, BitBoard> pin_rays;

	void Initialize();
	void GenerateRayMoves(const Square& from_square, const uint8_t dir, bool(BitBoard::*find_nearest_square)(Square&) const, const Piece active_piece, const BitBoard pin_ray_board, std::vector<Move>& moves) const;
	bool IsSquareAttacked(const Square& square, const uint8_t attacking_color) const;
	BitBoard GetCheckRayBoard(const Square king_square, const uint8_t dir, bool(BitBoard::* find_nearest_square)(Square &) const, const uint8_t rook_or_bishop_type) const;
	bool IsAttackedFromDirection(const Square square, const uint8_t dir, bool(BitBoard::*find_nearest_square)(Square&) const, const uint8_t rook_or_bishop_type, const uint8_t attacking_color) const;
	bool CanCastle(const int castling_index) const;
	BitBoard GenerateDangerRayBoard(const Square from_square, const uint8_t dir, bool(BitBoard::* find_nearest_square)(Square &) const, const BitBoard exclude_board) const;
	void GeneratePinInfo();
	void GeneratePinRayInfo(const Square king_square, const uint8_t dir, bool(BitBoard::*find_nearest_square)(Square&) const, const uint8_t rook_or_bishop_type);
	bool CheckEpDiscoveredCheck(const Square ep_square, const Square capturing_square) const;
};

