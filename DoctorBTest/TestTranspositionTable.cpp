#include "stdafx.h"
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "TranspositionTable.h"
#include "Position.h"
#include "Parser.h"
#include "HistoryMap.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DoctorBTest {
	TEST_CLASS(TestTranspositionTable) {
public:
	TEST_METHOD(TestTranspositionTableAddFindTrue) {
		//arrange
		Position pos;
		HistoryMap history;
		vector<string> tokens = { "startpos" };
		Parser::ParsePosition(tokens, pos, history);
		Move move = Move(Piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE), Square(Square::E2), Square(Square::E4));

		//act
		uint64_t hash_key = pos.GetHashKey();
		TranspositionTable::GetInstance().SetBestMove(hash_key, move);
		Move tt_move;
		bool found = TranspositionTable::GetInstance().FindBestMove(hash_key, tt_move);

		//assert
		Assert::IsTrue(found);
		Assert::AreEqual(move, tt_move);
	}

	TEST_METHOD(TestTranspositionTableAddFindFalse) {
		//arrange
		Position pos_1;
		HistoryMap history;
		vector<string> tokens = { "startpos" };
		Parser::ParsePosition(tokens, pos_1, history);
		Move move = Move(Piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE), Square(Square::E2), Square(Square::E4));

		//act
		uint64_t hash_key_1 = pos_1.GetHashKey();
		TranspositionTable::GetInstance().SetBestMove(hash_key_1, move);
		Position pos_2(pos_1);
		pos_2.ApplyMove(move);
		uint64_t hash_key_2 = pos_2.GetHashKey();

		Move actual_move;
		bool found = TranspositionTable::GetInstance().FindBestMove(hash_key_2, actual_move);

		//assert
		Assert::IsFalse(found);
	}

	};
}