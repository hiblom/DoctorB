#include "stdafx.h"
#include "Parser.h"
#include <boost/algorithm/string.hpp>
#include <boost/optional.hpp>
#include "Constants.h"
#include "MoveGenerator.h"

using namespace std;
using namespace boost::algorithm;

bool Parser::parsePosition(const vector<string>& position_tokens, Position& position, HistoryMap& history) {
	vector<string>::const_iterator moves_it = find(position_tokens.begin(), position_tokens.end(), "moves");

	if (iequals(position_tokens[0], "startpos")) {
		setStartPos(position);
	}
	else if (iequals(position_tokens[0], "fen")) {
		if (position_tokens.size() < 3)
			return false;

		vector<string> fen_tokens(position_tokens.begin() + 1, moves_it);
		if (!parseFen(fen_tokens, position)) 
			return false;
	}

	history.clear();
	history.increase(position.getHashKey());

	//parse the moves
	if (moves_it < position_tokens.end()) {
		vector<string> move_tokens(moves_it + 1, position_tokens.end());
		if (!parsePositionMoves(move_tokens, position, history))
			return false;
	}

	return true;
}

bool Parser::parsePiece(char c, Piece& piece) {
	size_t value = Constants::PIECE_CHARS.find(c);
	if (value == std::string::npos)
		return false;

	piece = Piece(static_cast<uint8_t>(value));
	return true;
}

bool Parser::parseFen(const vector<string>& position_tokens, Position& position) {
	size_t len = position_tokens.size();
	if (len < 1)
		return false;

	if (!parseFenPieces(position_tokens[0], position))
		return false;

	string color = len < 2 ? "w" : position_tokens[1];
	if (!parseFenActiveColor(color, position))
		return false;

	string castling_status = len < 3 ? "-" : position_tokens[2];
	if (!parseFenCastlingStatus(castling_status, position))
		return false;

	string ep_square = len < 4 ? "-" : position_tokens[3];
	if (!parseFenEpSquare(ep_square, position))
		return false;

	string halfmove_clock = len < 5 ? "0" : position_tokens[4];
	if (!parseFenHalfmoveClock(halfmove_clock, position))
		return false;

	return true;
}

bool Parser::parseFenPieces(string text, Position& position) {
	uint8_t y = 7;
	uint8_t x = 0;

	vector<string> tokens;
	split(tokens, text, boost::is_any_of("/"));

	if (tokens.size() != 8)
		return false;

	Piece piece;
	for (string token : tokens) {
		for (char c : token) {
			if ('1' <= c && c <= '8') {
				x += (c - '0');
			}
			else {
				if (parsePiece(c, piece)) {
					position.setPiece(Square(x, y), piece);
					x++;
				}
				else {
					return false;
				}
			}
		}
		x = 0;
		y--;
	}
	return true;
}

bool Parser::parseFenActiveColor(string text, Position& position) {
	if (text == "w") {
		position.setActiveColor(Piece::COLOR_WHITE);
		return true;
	}
	else if (text == "b") {
		position.setActiveColor(Piece::COLOR_BLACK);
		return true;
	}
	return false;
}

bool Parser::parseFenCastlingStatus(string text, Position & position) {
	position.setCastlingStatus(Constants::CASTLING_WHITE_KINGSIDE, text.find('K') != string::npos);
	position.setCastlingStatus(Constants::CASTLING_WHITE_QUEENSIDE, text.find('Q') != string::npos);
	position.setCastlingStatus(Constants::CASTLING_BLACK_KINGSIDE, text.find('k') != string::npos);
	position.setCastlingStatus(Constants::CASTLING_BLACK_QUEENSIDE, text.find('q') != string::npos);
	return true;
}

bool Parser::parseFenEpSquare(string text, Position & position) {
	if (text == "-") {
		position.resetEpSquare();
		return true;
	}
	else {
		Square ep_square;
		if (parseSquare(text, ep_square)) {
			position.setEpSquare(ep_square);
			return true;
		}
	}
	return false;
}

bool Parser::parseFenHalfmoveClock(string text, Position & position) {
	try {
		int value = stoi(text);
		if (value < 0 || value > UINT16_MAX) {
			return false;
		}
		position.setHalfmoveClock(value);
		return true;
	}
	catch (exception) {
		return false;
	}
}

bool Parser::parseSquare(string text, Square& square) {
	if (text.size() != 2)
		return false;
	
	uint8_t x;
	if ('A' <= text[0] && text[0] <= 'H')
		x = text[0] - 'A';
	else if ('a' <= text[0] && text[0] <= 'h')
		x = text[0] - 'a';
	else
		return false;

	uint8_t y;
	if ('1' <= text[1] && text[1] <= '8')
		y = text[1] - '1';
	else
		return false;

	square.setXy(x, y);
	return true;
}

bool Parser::parsePositionMoves(const vector<string>& tokens, Position& position, HistoryMap& history) {
	Move move;
	for (string token : tokens) {
		if (!parseMove(token, move))
			return false;

		MoveGenerator move_gen(position);
		if (!move_gen.setMoveFlags(move))
			return false;

		if (!position.applyMove(move))
			return false;

		if (position.getHalfmoveClock() == 0)
			history.clear();

		history.increase(position.getHashKey());
	}
	return true;
}

bool Parser::parseMove(std::string text, Move& move) {
	size_t len = text.size();
	if (len < 4 || len > 5)
		return false;

	Square square_from;
	if (!parseSquare(text.substr(0, 2), square_from))
		return false;
	
	Square square_to;
	if (!parseSquare(text.substr(2, 2), square_to))
		return false;

	Piece promo_piece;
	if (len == 4) {
		move = Move(square_from, square_to);
	}
	else {
		Piece promo_piece;
		if (!parsePiece(text[4], promo_piece))
			return false;

		move = Move(square_from, square_to, promo_piece);
	}

	return true;
}

void Parser::setStartPos(Position& position) {
	position.setPiece(Square(Square::A1), Piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE));
	position.setPiece(Square(Square::B1), Piece(Piece::TYPE_KNIGHT, Piece::COLOR_WHITE));
	position.setPiece(Square(Square::C1), Piece(Piece::TYPE_BISHOP, Piece::COLOR_WHITE));
	position.setPiece(Square(Square::D1), Piece(Piece::TYPE_QUEEN, Piece::COLOR_WHITE));
	position.setPiece(Square(Square::E1), Piece(Piece::TYPE_KING, Piece::COLOR_WHITE));
	position.setPiece(Square(Square::F1), Piece(Piece::TYPE_BISHOP, Piece::COLOR_WHITE));
	position.setPiece(Square(Square::G1), Piece(Piece::TYPE_KNIGHT, Piece::COLOR_WHITE));
	position.setPiece(Square(Square::H1), Piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE));

	position.setPiece(Square(Square::A2), Piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE));
	position.setPiece(Square(Square::B2), Piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE));
	position.setPiece(Square(Square::C2), Piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE));
	position.setPiece(Square(Square::D2), Piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE));
	position.setPiece(Square(Square::E2), Piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE));
	position.setPiece(Square(Square::F2), Piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE));
	position.setPiece(Square(Square::G2), Piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE));
	position.setPiece(Square(Square::H2), Piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE));

	position.setPiece(Square(Square::A7), Piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK));
	position.setPiece(Square(Square::B7), Piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK));
	position.setPiece(Square(Square::C7), Piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK));
	position.setPiece(Square(Square::D7), Piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK));
	position.setPiece(Square(Square::E7), Piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK));
	position.setPiece(Square(Square::F7), Piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK));
	position.setPiece(Square(Square::G7), Piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK));
	position.setPiece(Square(Square::H7), Piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK));

	position.setPiece(Square(Square::A8), Piece(Piece::TYPE_ROOK, Piece::COLOR_BLACK));
	position.setPiece(Square(Square::B8), Piece(Piece::TYPE_KNIGHT, Piece::COLOR_BLACK));
	position.setPiece(Square(Square::C8), Piece(Piece::TYPE_BISHOP, Piece::COLOR_BLACK));
	position.setPiece(Square(Square::D8), Piece(Piece::TYPE_QUEEN, Piece::COLOR_BLACK));
	position.setPiece(Square(Square::E8), Piece(Piece::TYPE_KING, Piece::COLOR_BLACK));
	position.setPiece(Square(Square::F8), Piece(Piece::TYPE_BISHOP, Piece::COLOR_BLACK));
	position.setPiece(Square(Square::G8), Piece(Piece::TYPE_KNIGHT, Piece::COLOR_BLACK));
	position.setPiece(Square(Square::H8), Piece(Piece::TYPE_ROOK, Piece::COLOR_BLACK));

	position.setActiveColor(Piece::COLOR_WHITE);

	position.setCastlingStatus(Constants::CASTLING_WHITE_KINGSIDE, true);
	position.setCastlingStatus(Constants::CASTLING_WHITE_QUEENSIDE, true);
	position.setCastlingStatus(Constants::CASTLING_BLACK_KINGSIDE, true);
	position.setCastlingStatus(Constants::CASTLING_BLACK_QUEENSIDE, true);

	position.resetEpSquare();

	position.setHalfmoveClock(0);
}

