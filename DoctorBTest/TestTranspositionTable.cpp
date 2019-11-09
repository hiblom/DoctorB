#include "stdafx.h"
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "TranspositionTable.h"
#include "Position.h"
#include "Parser.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DoctorBTest {
	TEST_CLASS(TestTranspositionTable) {
public:
	TEST_METHOD(TestTranspositionTableAddFindTrue) {
		//arrange
		Position pos;
		vector<string> tokens = { "startpos" };
		Parser::ParsePosition(tokens, pos);
		Move move = Move(Piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE), Square(Square::E2), Square(Square::E4));

		//act
		uint64_t hash_key = pos.GetHashKey();
		TranspositionTable::GetInstance().AddEntry(hash_key, TranspositionTable::Entry(move));
		TranspositionTable::Entry actual_entry;
		bool found = TranspositionTable::GetInstance().FindEntry(hash_key, actual_entry);

		//assert
		Assert::IsTrue(found);
		Assert::AreEqual(move, actual_entry.best_move);
	}

	TEST_METHOD(TestTranspositionTableAddFindFalse) {
		//arrange
		Position pos_1;
		vector<string> tokens = { "startpos" };
		Parser::ParsePosition(tokens, pos_1);
		Move move = Move(Piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE), Square(Square::E2), Square(Square::E4));

		//act
		uint64_t hash_key_1 = pos_1.GetHashKey();
		TranspositionTable::GetInstance().AddEntry(hash_key_1, TranspositionTable::Entry(move));
		Position pos_2(pos_1);
		pos_2.ApplyMove(move);
		uint64_t hash_key_2 = pos_2.GetHashKey();

		TranspositionTable::Entry actual_entry;
		bool found = TranspositionTable::GetInstance().FindEntry(hash_key_2, actual_entry);

		//assert
		Assert::IsFalse(found);
	}

	};
}