#pragma once
#include <string>
#include <vector>
#include "Position.h"
#include "Piece.h"
#include "Square.h"
#include "Move.h"
#include "HistoryMap.h"

namespace Parser
{
	bool ParsePosition(const std::vector<std::string>& position_tokens, Position& position, HistoryMap& history);
	bool ParsePiece(char c, Piece& piece);
	bool ParseFen(const std::vector<std::string>& position_tokens, Position& position);
	bool ParseFenPieces(std::string text, Position& position);
	bool ParseFenActiveColor(std::string text, Position& position);
	bool ParseFenCastlingStatus(std::string text, Position& position);
	bool ParseFenEpSquare(std::string text, Position& position);
	bool ParseFenHalfmoveClock(std::string text, Position& position);
	bool ParseSquare(std::string text, Square& square);
	bool ParsePositionMoves(const std::vector<std::string>& tokens, Position& position, HistoryMap& history);
	bool ParseMove(std::string text, Move& square);
	void SetStartPos(Position& position);
};