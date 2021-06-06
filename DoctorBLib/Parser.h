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
	bool parsePosition(const std::vector<std::string>& position_tokens, Position& position, HistoryMap& history);
	bool parsePiece(char c, Piece& piece);
	bool parseFen(const std::vector<std::string>& position_tokens, Position& position);
	bool parseFenPieces(std::string text, Position& position);
	bool parseFenActiveColor(std::string text, Position& position);
	bool parseFenCastlingStatus(std::string text, Position& position);
	bool parseFenEpSquare(std::string text, Position& position);
	bool parseFenHalfmoveClock(std::string text, Position& position);
	bool parseSquare(std::string text, Square& square);
	bool parsePositionMoves(const std::vector<std::string>& tokens, Position& position, HistoryMap& history);
	bool parseMove(std::string text, Move& square);
	void setStartPos(Position& position);
};