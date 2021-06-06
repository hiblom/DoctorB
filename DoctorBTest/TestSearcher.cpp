#include "stdafx.h"
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "Searcher.h"
#include "MoveBoard.h"
#include "Position.h"
#include "Parser.h"
#include "HistoryMap.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DoctorBTest {
	TEST_CLASS(TestSearcher) {
public:
	TEST_METHOD(TestPieceSearcherGo) {
		//arrange
		Position pos;
		HistoryMap history;
		Parser::parsePosition({ "startpos" }, pos, history);
		Searcher search(pos, history);

		//act
		search.goDepth(2);

		//assert
	}


	};
}