#include "stdafx.h"
#include <string>
#include <vector>
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "Position.h"
#include "Square.h"
#include "Piece.h"
#include "Parser.h"
#include "Constants.h"
#include "Parser.h"
#include "Zobrist.h"
#include "HistoryMap.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DoctorBTest {
	TEST_CLASS(TestPosition) {
public:
	TEST_METHOD(TestPositionSetGetPiece) {
		//arrange
		Position pos;
		Square square(Square::E1);
		Piece piece(Piece::TYPE_KING, Piece::COLOR_WHITE);
		Piece actual_piece;

		//act
		pos.SetPiece(square, piece);
		bool result = pos.GetPiece(square, actual_piece);

		//assert
		Assert::IsTrue(result);
		Assert::AreEqual(piece, actual_piece);
	}

	TEST_METHOD(TestPositionClearSquare) {
		//arrange
		Position pos;
		HistoryMap history;
		Parser::ParsePosition({ "startpos" }, pos, history);
		Square square(Square::E1);
		Piece actual_piece;

		//act
		pos.ClearSquare(square);
		bool result = pos.GetPiece(square, actual_piece);

		//assert
		Assert::IsFalse(result);
	}

	TEST_METHOD(TestPositionClearSquarePiece) {
		//arrange
		Position pos;
		HistoryMap history;
		Parser::ParsePosition({ "startpos" }, pos, history);
		Square square(Square::E8);
		Piece piece(Piece::TYPE_KING, Piece::COLOR_BLACK);
		Piece actual_piece;

		//act
		pos.ClearSquare(square, piece);
		bool result = pos.GetPiece(square, actual_piece);

		//assert
		Assert::IsFalse(result);
	}


	TEST_METHOD(TestPositionActiveColor) {
		//arrange
		Position pos;
		uint8_t color = Piece::COLOR_BLACK;

		//act
		pos.SetActiveColor(color);
		uint8_t actual_color = pos.GetActiveColor();

		//assert
		Assert::AreEqual(color, actual_color);
	}

	TEST_METHOD(TestPositionCastlingStatus) {
		//arrange
		Position pos;
		int index = Constants::CASTLING_BLACK_QUEENSIDE;
		bool value = true;

		//act
		pos.SetCastlingStatus(index, value);
		bool actual_value = pos.GetCastlingStatus(index);

		//assert
		Assert::AreEqual(value, actual_value);
	}

	TEST_METHOD(TestPositionEpSquare) {
		//arrange
		Position pos;
		Square ep_square(Square::D3);
		Square actual_ep_square;

		//act
		pos.SetEpSquare(ep_square);
		bool result = pos.GetEpSquare(actual_ep_square);

		//assert
		Assert::IsFalse(result); //note: polyglot hashing: ep square will not be set when there is no enemy pawn next to moved pawn
		//Assert::AreEqual(ep_square, actual_ep_square);
	}

	TEST_METHOD(TestPositionResetEpSquare) {
		//arrange
		Position pos;
		Square ep_square(Square::D3);
		Square actual_ep_square;

		//act
		pos.SetEpSquare(ep_square);
		pos.ResetEpSquare();
		bool result = pos.GetEpSquare(actual_ep_square);

		//assert
		Assert::IsFalse(result);
	}

	TEST_METHOD(TestPositionHalfmoveClock) {
		//arrange
		Position pos;
		uint16_t hmc = 12;

		//act
		pos.SetHalfmoveClock(hmc);
		uint16_t actual_hmc = pos.GetHalfmoveClock();

		//assert
		Assert::AreEqual(static_cast<int>(hmc), static_cast<int>(actual_hmc));
	}

	TEST_METHOD(TestPositionGetPieceSquares) {
		//arrange
		Position pos;
		Piece piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK);
		Square square1(Square::A7);
		Square square2(Square::B7);
		vector<Square> actual_squares;

		//act
		pos.SetPiece(square1, piece);
		pos.SetPiece(square2, piece);
		bool result = pos.GetPieceSquares(piece, actual_squares);

		//assert
		Assert::AreEqual(2Ui64, actual_squares.size());
		Assert::IsTrue(find(actual_squares.begin(), actual_squares.end(), square1) != actual_squares.end());
		Assert::IsTrue(find(actual_squares.begin(), actual_squares.end(), square2) != actual_squares.end());
	}

	TEST_METHOD(TestPositionGenerateHashKey) {
		//arrange
		Position pos;
		HistoryMap history;
		Parser::ParsePosition({ "startpos" }, pos, history);

		//act
		uint64_t pos_hash_key = pos.GetHashKey();
		pos.RecalculateHashKey();
		uint64_t generated_hash_key = pos.GetHashKey();

		//assert
		Assert::AreEqual(pos_hash_key, generated_hash_key);
	}

	TEST_METHOD(TestPositionGenerateHashKey2) {
		//arrange
		Position pos;
		HistoryMap history;
		vector<string> tokens = { "startpos", "moves", "b1c3", "e7e5", "g1f3", "b8c6", "d2d4", "e5d4", "f3d4", "g8f6", "c1f4", "f8b4", "d1d3" };
		Parser::ParsePosition(tokens, pos, history);

		//act
		uint64_t pos_hash_key = pos.GetHashKey();
		pos.RecalculateHashKey();
		uint64_t generated_hash_key = pos.GetHashKey();

		//assert
		Assert::AreEqual(pos_hash_key, generated_hash_key);
	}

	TEST_METHOD(TestPositionPolyglotHash1) {
		//arrange
		Position pos;
		HistoryMap history;
		Parser::ParsePosition({ "startpos" }, pos, history);
		uint64_t expected_key = 0x463b96181691fc9cUi64;

		//act
		uint64_t actual_key = pos.GetHashKey();

		//assert
		Assert::AreEqual(actual_key, expected_key);
	}

	TEST_METHOD(TestPositionPolyglotHash2) {
		//arrange
		Position pos;
		HistoryMap history;
		vector<string> tokens = { "startpos", "moves", "e2e4" };
		Parser::ParsePosition(tokens, pos, history);
		uint64_t expected_key = 0x823c9b50fd114196Ui64;

		//act
		uint64_t actual_key = pos.GetHashKey();

		//assert
		Assert::AreEqual(actual_key, expected_key);
	}

	TEST_METHOD(TestPositionPolyglotHash3) {
		//arrange
		Position pos;
		HistoryMap history;
		vector<string> tokens = { "startpos", "moves", "e2e4", "d7d5" };
		Parser::ParsePosition(tokens, pos, history);
		uint64_t expected_key = 0x0756b94461c50fb0Ui64;

		//act
		uint64_t actual_key = pos.GetHashKey();

		//assert
		Assert::AreEqual(actual_key, expected_key);
	}

	TEST_METHOD(TestPositionPolyglotHash4) {
		//arrange
		Position pos;
		HistoryMap history;
		vector<string> tokens = { "startpos", "moves", "e2e4", "d7d5", "e4e5" };
		Parser::ParsePosition(tokens, pos, history);
		uint64_t expected_key = 0x662fafb965db29d4Ui64;

		//act
		uint64_t actual_key = pos.GetHashKey();

		//assert
		Assert::AreEqual(actual_key, expected_key);
	}

	TEST_METHOD(TestPositionPolyglotHash5) {
		//arrange
		Position pos;
		HistoryMap history;
		vector<string> tokens = { "startpos", "moves", "e2e4", "d7d5", "e4e5", "f7f5" };
		Parser::ParsePosition(tokens, pos, history);
		uint64_t expected_key = 0x22a48b5a8e47ff78Ui64;

		//act
		uint64_t actual_key = pos.GetHashKey();

		//assert
		Assert::AreEqual(actual_key, expected_key);
	}

	TEST_METHOD(TestPositionPolyglotHash6) {
		//arrange
		Position pos;
		HistoryMap history;
		vector<string> tokens = { "startpos", "moves", "e2e4", "d7d5", "e4e5", "f7f5", "e1e2" };
		Parser::ParsePosition(tokens, pos, history);
		uint64_t expected_key = 0x652a607ca3f242c1Ui64;

		//act
		uint64_t actual_key = pos.GetHashKey();

		//assert
		Assert::AreEqual(actual_key, expected_key);
	}

	TEST_METHOD(TestPositionPolyglotHash7) {
		//arrange
		Position pos;
		HistoryMap history;
		vector<string> tokens = { "startpos", "moves", "e2e4", "d7d5", "e4e5", "f7f5", "e1e2", "e8f7" };
		Parser::ParsePosition(tokens, pos, history);
		uint64_t expected_key = 0x00fdd303c946bdd9Ui64;

		//act
		uint64_t actual_key = pos.GetHashKey();

		//assert
		Assert::AreEqual(actual_key, expected_key);
	}

	TEST_METHOD(TestPositionPolyglotHash8) {
		//arrange
		Position pos;
		HistoryMap history;
		vector<string> tokens = { "startpos", "moves", "a2a4", "b7b5", "h2h4", "b5b4", "c2c4" };
		Parser::ParsePosition(tokens, pos, history);
		uint64_t expected_key = 0x3c8123ea7b067637Ui64;

		//act
		uint64_t actual_key = pos.GetHashKey();

		//assert
		Assert::AreEqual(actual_key, expected_key);
	}

	TEST_METHOD(TestPositionPolyglotHash9) {
		//arrange
		Position pos;
		HistoryMap history;
		vector<string> tokens = { "startpos", "moves", "a2a4", "b7b5", "h2h4", "b5b4", "c2c4", "b4c3", "a1a3" };
		Parser::ParsePosition(tokens, pos, history);
		uint64_t expected_key = 0x5c3f9b829b279560Ui64;

		//act
		uint64_t actual_key = pos.GetHashKey();

		//assert
		Assert::AreEqual(actual_key, expected_key);
	}

	};
}