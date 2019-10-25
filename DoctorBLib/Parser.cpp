#include "stdafx.h"
#include "Parser.h"
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/optional.hpp>
#include "Piece.h"

using namespace std;

bool Parser::ParsePosition(const vector<string>& position_tokens, Position& position) {
	vector<string>::const_iterator moves_it = find(position_tokens.begin(), position_tokens.end(), "moves");

	if (position_tokens[0] == "startpos") {
		SetStartPos(position);
	}
	else if (position_tokens[0] == "fen") {
		if (position_tokens.size() < 3)
			return false;

		vector<string> fen_tokens(position_tokens.begin() + 1, moves_it);
		if (!ParseFen(fen_tokens, position)) 
			return false;
	}

	//parse the moves
	if (moves_it < position_tokens.end()) {
		vector<string> move_tokens(moves_it + 1, position_tokens.end());
		if (!ParsePositionMoves(move_tokens, position))
			return false;
	}

	return true;
}

bool Parser::ParsePiece(char c, Piece& piece) {
	switch (c) {
	case 'P':
		piece = Piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE);
		return true;
	case 'p':
		piece = Piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK);
		return true;
	case 'K':
		piece = Piece(Piece::TYPE_KING, Piece::COLOR_WHITE);
		return true;
	case 'k':
		piece = Piece(Piece::TYPE_KING, Piece::COLOR_BLACK);
		return true;
	case 'Q':
		piece = Piece(Piece::TYPE_QUEEN, Piece::COLOR_WHITE);
		return true;
	case 'q':
		piece = Piece(Piece::TYPE_QUEEN, Piece::COLOR_BLACK);
		return true;
	case 'R':
		piece = Piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE);
		return true;
	case 'r':
		piece = Piece(Piece::TYPE_ROOK, Piece::COLOR_BLACK);
		return true;
	case 'B':
		piece = Piece(Piece::TYPE_BISHOP, Piece::COLOR_WHITE);
		return true;
	case 'b':
		piece = Piece(Piece::TYPE_BISHOP, Piece::COLOR_BLACK);
		return true;
	case 'N':
		piece = Piece(Piece::TYPE_KNIGHT, Piece::COLOR_WHITE);
		return true;
	case 'n':
		piece = Piece(Piece::TYPE_KNIGHT, Piece::COLOR_BLACK);
		return true;
	default:
		return false;
	}
}

bool Parser::ParseFen(const vector<string>& position_tokens, Position& position) {
	size_t len = position_tokens.size();
	if (len < 1)
		return false;

	if (!ParseFenPieces(position_tokens[0], position))
		return false;

	string color = len < 2 ? "w" : position_tokens[1];
	if (!ParseFenActiveColor(color, position))
		return false;

	string castling_status = len < 3 ? "-" : position_tokens[2];
	if (!ParseFenCastlingStatus(castling_status, position))
		return false;

	string ep_square = len < 4 ? "-" : position_tokens[3];
	if (!ParseFenEpSquare(ep_square, position))
		return false;

	string halfmove_clock = len < 5 ? "0" : position_tokens[4];
	if (!ParseFenHalfmoveClock(halfmove_clock, position))
		return false;

	string fullmove_number = len < 6 ? "1" : position_tokens[5];
	if (!ParseFenFullmoveNumber(fullmove_number, position))
		return false;

	return true;
}

bool Parser::ParseFenPieces(string text, Position& position) {
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
				if (ParsePiece(c, piece)) {
					position.SetPiece(Square(x, y), piece);
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

bool Parser::ParseFenActiveColor(string text, Position& position) {
	if (text == "w") {
		position.SetActiveColor(Piece::COLOR_WHITE);
		return true;
	}
	else if (text == "b") {
		position.SetActiveColor(Piece::COLOR_BLACK);
		return true;
	}
	return false;
}

bool Parser::ParseFenCastlingStatus(string text, Position & position) {
	position.SetCastlingStatus(0, text.find('K') != string::npos);
	position.SetCastlingStatus(1, text.find('Q') != string::npos);
	position.SetCastlingStatus(2, text.find('k') != string::npos);
	position.SetCastlingStatus(3, text.find('q') != string::npos);
	return true;
}

bool Parser::ParseFenEpSquare(string text, Position & position) {
	if (text == "-") {
		position.ResetEpSquare();
		return true;
	}
	else {
		Square ep_square;
		if (ParseSquare(text, ep_square)) {
			position.SetEpSquare(ep_square);
			return true;
		}
	}
	return false;
}

bool Parser::ParseFenHalfmoveClock(string text, Position & position) {
	try {
		int value = stoi(text);
		if (value < 0 || value > UINT16_MAX) {
			return false;
		}
		position.SetHalfmoveClock(value);
		return true;
	}
	catch (exception) {
		return false;
	}
}

bool Parser::ParseFenFullmoveNumber(string text, Position& position) {
	try {
		int value = stoi(text);
		if (value < 0 || value > UINT16_MAX) {
			return false;
		}
		position.SetFullmoveNumber(value);
		return true;
	}
	catch (exception) {
		return false;
	}
}

bool Parser::ParseSquare(string text, Square& square) {
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

	square.SetXy(x, y);
	return true;
}

bool Parser::ParsePositionMoves(const vector<string>& tokens, Position& position) {
	Move move;
	for (string token : tokens) {
		if (!ParseMove(token, move))
			return false;
		if (!position.ApplyMove(move))
			return false;
	}
	return true;
}

bool Parser::ParseMove(std::string text, Move& move) {
	size_t len = text.size();
	if (len < 4 || len > 5)
		return false;

	Square square_from;
	if (!ParseSquare(text.substr(0, 2), square_from))
		return false;
	
	Square square_to;
	if (!ParseSquare(text.substr(2, 2), square_to))
		return false;

	Piece promo_piece;
	if (len == 4) {
		move = Move(square_from, square_to);
	}
	else {
		Piece promo_piece;
		if (!ParsePiece(text[4], promo_piece))
			return false;

		move = Move(square_from, square_to, promo_piece);
	}

	return true;
}

void Parser::SetStartPos(Position& position) {
	position.SetPiece(Square(Square::A1), Piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE));
	position.SetPiece(Square(Square::B1), Piece(Piece::TYPE_KNIGHT, Piece::COLOR_WHITE));
	position.SetPiece(Square(Square::C1), Piece(Piece::TYPE_BISHOP, Piece::COLOR_WHITE));
	position.SetPiece(Square(Square::D1), Piece(Piece::TYPE_QUEEN, Piece::COLOR_WHITE));
	position.SetPiece(Square(Square::E1), Piece(Piece::TYPE_KING, Piece::COLOR_WHITE));
	position.SetPiece(Square(Square::F1), Piece(Piece::TYPE_BISHOP, Piece::COLOR_WHITE));
	position.SetPiece(Square(Square::G1), Piece(Piece::TYPE_KNIGHT, Piece::COLOR_WHITE));
	position.SetPiece(Square(Square::H1), Piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE));

	position.SetPiece(Square(Square::A2), Piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE));
	position.SetPiece(Square(Square::B2), Piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE));
	position.SetPiece(Square(Square::C2), Piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE));
	position.SetPiece(Square(Square::D2), Piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE));
	position.SetPiece(Square(Square::E2), Piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE));
	position.SetPiece(Square(Square::F2), Piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE));
	position.SetPiece(Square(Square::G2), Piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE));
	position.SetPiece(Square(Square::H2), Piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE));

	position.SetPiece(Square(Square::A7), Piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK));
	position.SetPiece(Square(Square::B7), Piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK));
	position.SetPiece(Square(Square::C7), Piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK));
	position.SetPiece(Square(Square::D7), Piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK));
	position.SetPiece(Square(Square::E7), Piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK));
	position.SetPiece(Square(Square::F7), Piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK));
	position.SetPiece(Square(Square::G7), Piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK));
	position.SetPiece(Square(Square::H7), Piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK));

	position.SetPiece(Square(Square::A8), Piece(Piece::TYPE_ROOK, Piece::COLOR_BLACK));
	position.SetPiece(Square(Square::B8), Piece(Piece::TYPE_KNIGHT, Piece::COLOR_BLACK));
	position.SetPiece(Square(Square::C8), Piece(Piece::TYPE_BISHOP, Piece::COLOR_BLACK));
	position.SetPiece(Square(Square::D8), Piece(Piece::TYPE_QUEEN, Piece::COLOR_BLACK));
	position.SetPiece(Square(Square::E8), Piece(Piece::TYPE_KING, Piece::COLOR_BLACK));
	position.SetPiece(Square(Square::F8), Piece(Piece::TYPE_BISHOP, Piece::COLOR_BLACK));
	position.SetPiece(Square(Square::G8), Piece(Piece::TYPE_KNIGHT, Piece::COLOR_BLACK));
	position.SetPiece(Square(Square::H8), Piece(Piece::TYPE_ROOK, Piece::COLOR_BLACK));

	position.SetActiveColor(Piece::COLOR_WHITE);

	position.SetCastlingStatus(0, true);
	position.SetCastlingStatus(1, true);
	position.SetCastlingStatus(2, true);
	position.SetCastlingStatus(3, true);

	position.ResetEpSquare();

	position.SetHalfmoveClock(0);

	position.SetFullmoveNumber(1);
}

