#include "stdafx.h"
#include <string>
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "Move.h"
#include "Square.h"
#include "Piece.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DoctorBTest {
	TEST_CLASS(TestMove) {
	public:
		TEST_METHOD(TestMoveConstructorFromTo) {
			//arrange
			Square square_from = Square(1Ui8, 2Ui8);
			Square square_to = Square(3Ui8, 4Ui8);
			Piece actual_promo_piece;

			//act
			Move move = Move(square_from, square_to);

			//assert
			Assert::AreEqual(square_from, move.GetSquareFrom());
			Assert::AreEqual(square_to, move.GetSquareTo());
			Assert::IsFalse(move.GetPromoPiece(actual_promo_piece));
		}

		TEST_METHOD(TestMoveConstructorFromToPromo) {
			//arrange
			Square square_from = Square(7Ui8, 7Ui8);
			Square square_to = Square(8Ui8, 7Ui8);
			Piece expected_promo_piece = Piece(Piece::TYPE_QUEEN, Piece::COLOR_WHITE);
			Piece actual_promo_piece;

			//act
			Move move = Move(square_from, square_to, expected_promo_piece);

			//assert
			Assert::AreEqual(square_from, move.GetSquareFrom());
			Assert::AreEqual(square_to, move.GetSquareTo());
			Assert::IsTrue(move.GetPromoPiece(actual_promo_piece));
			Assert::AreEqual(expected_promo_piece, actual_promo_piece);
		}

		TEST_METHOD(TestMoveEquals1) {
			//arrange
			Square square_from = Square(1Ui8, 2Ui8);
			Square square_to = Square(3Ui8, 4Ui8);
			Move move1 = Move(square_from, square_to);
			Move move2 = Move(square_from, square_to);

			//act
			bool equals = (move1 == move2);
			
			//assert
			Assert::IsTrue(equals);
		}

		TEST_METHOD(TestMoveEquals2) {
			//arrange
			Square square_from = Square(2Ui8, 7Ui8);
			Square square_to = Square(3Ui8, 7Ui8);
			Piece promo_piece = Piece(Piece::TYPE_KNIGHT, Piece::COLOR_WHITE);

			Move move1 = Move(square_from, square_to, promo_piece);
			Move move2 = Move(square_from, square_to, promo_piece);

			//act
			bool equals = (move1 == move2);

			//assert
			Assert::IsTrue(equals);
		}

		TEST_METHOD(TestMoveEquals3) {
			//arrange
			Square square_from = Square(2Ui8, 7Ui8);
			Square square_to = Square(3Ui8, 7Ui8);
			Piece promo_piece = Piece(Piece::TYPE_KNIGHT, Piece::COLOR_WHITE);

			Move move1 = Move(square_from, square_to, promo_piece);
			Move move2 = Move(square_from, square_to);

			//act
			bool equals = (move1 == move2);

			//assert
			Assert::IsFalse(equals);
		}

		TEST_METHOD(TestMoveToString) {
			//arrange
			Square square_from = Square(1Ui8, 2Ui8);
			Square square_to = Square(3Ui8, 4Ui8);
			Move move = Move(square_from, square_to);

			//act
			string notation = move.ToString();

			//assert
			Assert::AreEqual(string("b3d5"), notation);
		}

	};
}