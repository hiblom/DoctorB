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
	void generateMoves(std::vector<Move>& moves);
	void generateKingMoves(std::vector<Move>& moves) const;
	void generateKnightMoves(std::vector<Move>& moves) const;
	void generatePawnMoves(std::vector<Move>& moves) const;
	void generateRookMoves(std::vector<Move>& moves) const;
	void generateBishopMoves(std::vector<Move>& moves) const;
	void generateQueenMoves(std::vector<Move>& moves) const;
	void generateDangerBoard();
	bool isCheck();
	bool setMoveFlags(Move& move);
	void generateCheckInfo();
	uint8_t getCheckerCount();
	BitBoard getBlockBoard();
	bool getLvaCapture(const Square to_square, Move& move) const;
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

	void initialize();
	void generateRayMoves(const Square& from_square, const uint8_t dir, bool(BitBoard::*find_nearest_square)(Square&) const, const Piece active_piece, const BitBoard pin_ray_board, std::vector<Move>& moves) const;
	BitBoard getCheckRayBoard(const Square king_square, const uint8_t dir, bool(BitBoard::* find_nearest_square)(Square &) const, const uint8_t rook_or_bishop_type) const;
	bool canCastle(const int castling_index) const;
	BitBoard generateDangerRayBoard(const Square from_square, const uint8_t dir, bool(BitBoard::* find_nearest_square)(Square &) const, const BitBoard exclude_board) const;
	void generatePinInfo();
	void generatePinRayInfo(const Square king_square, const uint8_t dir, bool(BitBoard::*find_nearest_square)(Square&) const, const uint8_t rook_or_bishop_type);
	bool checkEpDiscoveredCheck(const Square ep_square, const Square capturing_square) const;
	bool isCapture(const Square to_square) const;
};

