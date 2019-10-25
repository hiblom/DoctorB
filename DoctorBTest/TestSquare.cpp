#include "stdafx.h"
#include "CppUnitTest.h"
#include "Square.h"
#include <string>

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DoctorBTest {
	TEST_CLASS(TestSquare) {
	public:
		TEST_METHOD(TestSquareConstructorXy) {
			//arrange
			uint8_t x = 3Ui8;
			uint8_t y = 4Ui8;

			//act
			Square square = Square(x, y);

			//assert
			Assert::AreEqual(uint8_t(y * 8 + x), square.GetValue());
		}

		TEST_METHOD(TestSquareSetXy) {
			//arrange
			uint8_t x = 3Ui8;
			uint8_t y = 4Ui8;

			//act
			Square square = Square();
			square.SetXy(x, y);

			//assert
			Assert::AreEqual(uint8_t(y * 8 + x), square.GetValue());
		}

		TEST_METHOD(TestSquareGetXy) {
			//arrange
			uint8_t x = 5Ui8;
			uint8_t y = 2Ui8;

			//act
			Square square = Square(x, y);

			//assert
			Assert::AreEqual(x, square.GetX());
			Assert::AreEqual(y, square.GetY());
		}

		TEST_METHOD(TestSquareEquals) {
			//arrange
			uint8_t x = 3Ui8;
			uint8_t y = 4Ui8;
			Square square1 = Square(x, y);
			Square square2 = Square(x, y);

			//act
			bool equal = (square1 == square2);

			//assert
			Assert::IsTrue(equal);
		}

		TEST_METHOD(TestSquareToString) {
			//arrange
			uint8_t x = 0Ui8;
			uint8_t y = 7Ui8;
			Square square = Square(x, y);

			//act
			string notation = square.ToString();

			//assert
			Assert::AreEqual(string("a8"), notation);
		}


	};
}