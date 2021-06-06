#include "stdafx.h"
#include "CppUnitTest.h"
#include "Piece.h"
#include <string>

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DoctorBTest {
	TEST_CLASS(TestPiece) {
	public:
		TEST_METHOD(TestPieceConstructorTypeColor) {
			//arrange
			uint8_t type = Piece::TYPE_KING;
			uint8_t color = Piece::COLOR_WHITE;

			//act
			Piece piece = Piece(type, color);

			//assert
			Assert::AreEqual(color, piece.getColor());
			Assert::AreEqual(type, piece.getType());
		}

		TEST_METHOD(TestPieceSetType) {
			//arrange
			uint8_t type = Piece::TYPE_PAWN;
			uint8_t new_type = Piece::TYPE_QUEEN;
			uint8_t color = Piece::COLOR_BLACK;
			Piece piece = Piece(type, color);

			//act
			piece.setType(new_type);

			//assert
			Assert::AreEqual(new_type, piece.getType());
		}
		
		TEST_METHOD(TestPieceSetColor) {
			//arrange
			uint8_t type = Piece::TYPE_KNIGHT;
			uint8_t color = Piece::COLOR_BLACK;
			uint8_t new_color = Piece::COLOR_WHITE;
			Piece piece = Piece(type, color);

			//act
			piece.setColor(new_color);

			//assert
			Assert::AreEqual(new_color, piece.getColor());
		}

		TEST_METHOD(TestPieceToChar) {
			//arrange
			uint8_t type = Piece::TYPE_BISHOP;
			uint8_t color = Piece::COLOR_BLACK;
			Piece piece = Piece(type, color);
			char expected_char = 'b';

			//act
			char c = piece.toChar();

			//assert
			Assert::AreEqual(expected_char, c);
		}

		TEST_METHOD(TestPieceToString) {
			//arrange
			uint8_t type = Piece::TYPE_QUEEN;
			uint8_t color = Piece::COLOR_WHITE;
			Piece piece = Piece(type, color);
			string expected = "Q";

			//act
			string actual = piece.toString();

			//assert
			Assert::AreEqual(expected, actual);
		}

		TEST_METHOD(TestPieceEquals) {
			//arrange
			uint8_t type = Piece::TYPE_KNIGHT;
			uint8_t color = Piece::COLOR_BLACK;
			Piece piece1 = Piece(type, color);
			Piece piece2 = Piece(type, color);

			//act
			bool equals = (piece1 == piece2);

			//assert
			Assert::IsTrue(equals);
		}
	};
}