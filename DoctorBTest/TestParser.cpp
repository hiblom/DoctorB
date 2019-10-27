#include "stdafx.h"
#include <string>
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "Parser.h"
#include "Move.h"
#include "Square.h"
#include "Piece.h"
#include "Constants.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DoctorBTest {
	TEST_CLASS(TestParser) {
public:
	TEST_METHOD(TestParserParsePiece) {
		//arrange
		Piece actual_piece;
		Piece expected_piece = Piece(Piece::TYPE_KING, Piece::COLOR_BLACK);

		//act
		Parser::ParsePiece('k', actual_piece);

		//assert
		Assert::AreEqual(expected_piece, actual_piece);
	}

	TEST_METHOD(TestParserParseSquare) {
		//arrange
		Square actual_square;
		Square expected_square = Square(4Ui8, 1Ui8);

		//act
		Parser::ParseSquare("e2", actual_square);

		//assert
		Assert::AreEqual(expected_square, actual_square);
	}

	TEST_METHOD(TestParserParseMove) {
		//arrange
		Move actual_move;

		Square square_from = Square(4Ui8, 1Ui8);
		Square square_to = Square(4Ui8, 3Ui8);
		Move expected_move = Move(square_from, square_to);

		//act
		Parser::ParseMove("e2e4", actual_move);

		//assert
		Assert::AreEqual(expected_move, actual_move);
	}

	TEST_METHOD(TestParserParseFenPieces) {
		//arrange
		Position pos;
		Piece actual_piece_d5;
		Piece actual_piece_a1;
		Square square = Square(3Ui8, 4Ui8);
		
		//act
		Parser::ParseFenPieces("8/8/8/3K4/8/8/8/8", pos);
		bool res_d5 = pos.GetPiece(Square(3Ui8, 4Ui8), actual_piece_d5);
		bool res_a1 = pos.GetPiece(Square(0Ui8, 0Ui8), actual_piece_a1);

		//assert
		Assert::IsTrue(res_d5);
		Assert::AreEqual(Piece(Piece::TYPE_KING, Piece::COLOR_WHITE), actual_piece_d5);
		Assert::IsFalse(res_a1);
	}

	TEST_METHOD(TestParserParseFenActiveColor) {
		//arrange
		Position pos;

		//act
		bool res = Parser::ParseFenActiveColor("b", pos);

		//assert
		Assert::IsTrue(res);
		Assert::AreEqual(Piece::COLOR_BLACK, pos.GetActiveColor());
	}

	TEST_METHOD(TestParserParseFenCastlingStatus1) {
		//arrange
		Position pos;

		//act
		bool res = Parser::ParseFenCastlingStatus("Kq", pos);

		//assert
		Assert::IsTrue(res);
		Assert::IsTrue(pos.GetCastlingStatus(Constants::CASTLING_WHITE_KINGSIDE));
		Assert::IsFalse(pos.GetCastlingStatus(Constants::CASTLING_WHITE_QUEENSIDE));
		Assert::IsFalse(pos.GetCastlingStatus(Constants::CASTLING_BLACK_KINGSIDE));
		Assert::IsTrue(pos.GetCastlingStatus(Constants::CASTLING_BLACK_QUEENSIDE));
	}

	TEST_METHOD(TestParserParseFenCastlingStatus2) {
		//arrange
		Position pos;

		//act
		bool res = Parser::ParseFenCastlingStatus("-", pos);

		//assert
		Assert::IsTrue(res);
		Assert::IsFalse(pos.GetCastlingStatus(Constants::CASTLING_WHITE_KINGSIDE));
		Assert::IsFalse(pos.GetCastlingStatus(Constants::CASTLING_WHITE_QUEENSIDE));
		Assert::IsFalse(pos.GetCastlingStatus(Constants::CASTLING_BLACK_KINGSIDE));
		Assert::IsFalse(pos.GetCastlingStatus(Constants::CASTLING_BLACK_QUEENSIDE));
	}


	TEST_METHOD(TestParserParseFenEpSquare1) {
		//arrange
		Position pos;
		Square ep_square;

		//act
		bool parse_res = Parser::ParseFenEpSquare("b3", pos);
		bool get_res = pos.GetEpSquare(ep_square);

		//assert
		Assert::IsTrue(parse_res);
		Assert::IsTrue(get_res);
		Assert::AreEqual(Square(1Ui8, 2Ui8), ep_square);
	}

	TEST_METHOD(TestParserParseFenEpSquare2) {
		//arrange
		Position pos;
		Square ep_square;

		//act
		bool parse_res = Parser::ParseFenEpSquare("-", pos);
		bool get_res = pos.GetEpSquare(ep_square);

		//assert
		Assert::IsTrue(parse_res);
		Assert::IsFalse(get_res);
	}

	TEST_METHOD(TestParserParseHalfmoveClock) {
		//arrange
		Position pos;

		//act
		bool parse_res = Parser::ParseFenHalfmoveClock("42", pos);

		//assert
		Assert::IsTrue(parse_res);
		Assert::AreEqual(42, (int)pos.GetHalfmoveClock());
	}

	TEST_METHOD(TestParserParseFen) {
		//arrange
		Position pos;
		vector<string> tokens = {"r1bqkb1r/ppp1pppp/2n5/4p3/3P2n1/5N1P/PPP2PP1/RNBQKB1R", "w", "KQkq", "-", "0", "6"};
		Square ep_square;

		//act
		bool parse_res = Parser::ParseFen(tokens, pos);

		Piece piece_c1;
		Piece piece_h6;
		bool get_res_c1 = pos.GetPiece(Square(2Ui8, 0Ui8), piece_c1);
		bool get_res_h6 = pos.GetPiece(Square(7Ui8, 5Ui8), piece_h6);

		//assert
		Assert::IsTrue(parse_res);
		Assert::IsTrue(get_res_c1);
		Assert::AreEqual(Piece(Piece::TYPE_BISHOP, Piece::COLOR_WHITE), piece_c1);
		Assert::IsFalse(get_res_h6);
		Assert::AreEqual(Piece::COLOR_WHITE, pos.GetActiveColor());
		Assert::IsTrue(pos.GetCastlingStatus(0));
		Assert::IsTrue(pos.GetCastlingStatus(1));
		Assert::IsTrue(pos.GetCastlingStatus(2));
		Assert::IsTrue(pos.GetCastlingStatus(3));
		Assert::IsFalse(pos.GetEpSquare(ep_square));
		Assert::AreEqual(0, (int)pos.GetHalfmoveClock());
	}

	TEST_METHOD(TestParserParsePosition) {
		//arrange
		Position pos;
		vector<string> tokens = { "startpos", "moves", "b1c3", "e7e5", "g1f3", "b8c6", "d2d4", "e5d4", "f3d4", "g8f6", "c1f4", "f8b4", "d1d3" };
		Square ep_square;

		//act
		bool parse_res = Parser::ParsePosition(tokens, pos);

		Piece piece_d3;
		Piece piece_b1;
		bool get_res_d3 = pos.GetPiece(Square(3Ui8, 2Ui8), piece_d3);
		bool get_res_b1 = pos.GetPiece(Square(1Ui8, 0Ui8), piece_b1);

		//assert
		Assert::IsTrue(parse_res);
		Assert::IsTrue(get_res_d3);
		Assert::AreEqual(Piece(Piece::TYPE_QUEEN, Piece::COLOR_WHITE), piece_d3);
		Assert::IsFalse(get_res_b1);
		Assert::AreEqual(Piece::COLOR_BLACK, pos.GetActiveColor());
		Assert::IsTrue(pos.GetCastlingStatus(Constants::CASTLING_WHITE_KINGSIDE));
		Assert::IsTrue(pos.GetCastlingStatus(Constants::CASTLING_WHITE_QUEENSIDE));
		Assert::IsTrue(pos.GetCastlingStatus(Constants::CASTLING_BLACK_KINGSIDE));
		Assert::IsTrue(pos.GetCastlingStatus(Constants::CASTLING_BLACK_QUEENSIDE));
		Assert::IsFalse(pos.GetEpSquare(ep_square));
		Assert::AreEqual(4, (int)pos.GetHalfmoveClock());
	}

	};
}