#include "stdafx.h"
#include <string>
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "Position.h"
#include "HistoryMap.h"
#include "Perft.h"
#include "Parser.h"
#include "MoveBoard.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DoctorBTest {
	TEST_CLASS(TestPerft) {
public:
	TEST_METHOD(TestPerftStartpos1) {
		//arrange
		Position pos;
		HistoryMap history;
		Parser::parsePosition({ "startpos" }, pos, history);
		Perft perft(pos);
		perft.setDepth(1);

		//act
		uint64_t count = perft.go();

		//assert
		Assert::AreEqual(20Ui64, count);
	}

	TEST_METHOD(TestPerftStartpos2) {
		//arrange
		Position pos;
		HistoryMap history;
		Parser::parsePosition({ "startpos" }, pos, history);
		Perft perft(pos);
		perft.setDepth(2);

		//act
		uint64_t count = perft.go();

		//assert
		Assert::AreEqual(400Ui64, count);
	}

	};
}