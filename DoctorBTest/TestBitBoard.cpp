#include "stdafx.h"
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "BitBoard.h"
#include <vector>

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DoctorBTest {		
	TEST_CLASS(TestBitBoard) {
	public:
		TEST_METHOD(TestBitBoardSet) {
			//arrange
			BitBoard bit_board = BitBoard();
			uint8_t square_value = 10Ui8;

			//act
			bit_board.Set(square_value);

			//assert
			Assert::IsTrue(bit_board.Check(square_value));
		}

		TEST_METHOD(TestBitBoardClear) {
			//arrange
			BitBoard bit_board = BitBoard(0xFFFFFFFFFFFFFFFFUi64);
			uint8_t square_value = 10Ui8;

			//act
			bit_board.Clear(square_value);

			//assert
			Assert::IsFalse(bit_board.Check(square_value));
		}

		//GetSquares
		TEST_METHOD(TestBitGetSquares) {
			//arrange
			BitBoard bit_board = BitBoard();
			Square square1 = Square(10Ui8);
			Square square2 = Square(20Ui8);
			bit_board.Set(square1.GetValue());
			bit_board.Set(square2.GetValue());
			vector<Square> squares;

			//act
			bit_board.GetSquares(squares);

			//assert
			Assert::AreEqual((size_t)2, squares.size());
			Assert::AreEqual(square1, squares[0]);
			Assert::AreEqual(square2, squares[1]);
		}

	};
}