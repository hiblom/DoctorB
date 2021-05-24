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
		Assert::IsTrue(result);
		Assert::AreEqual(ep_square, actual_ep_square);
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
		Assert::AreEqual((int)hmc, (int)actual_hmc);
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

	TEST_METHOD(TestPositionStartingHashKey) {
		//arrange
		Position pos;
		HistoryMap history;
		Parser::ParsePosition({ "startpos" }, pos, history);
		uint64_t expected_key = 0x463b96181691fc9cULL;

		//act
		uint64_t actual_key = pos.GetHashKey();


		//assert
		Assert::AreEqual(actual_key, expected_key);
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

	};
}