#include "stdafx.h"
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "Searcher.h"
#include "MoveBoard.h"
#include "Position.h"
#include "Parser.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DoctorBTest {
	TEST_CLASS(TestSearcher) {
public:
	TEST_METHOD(TestPieceSearcherGo) {
		//arrange
		Position pos;
		Parser::ParsePosition({ "startpos" }, pos);
		Searcher search(pos);

		//act
		search.GoDepth(2);

		//assert
	}


	};
}