#include "stdafx.h"
#include <string>
#include <algorithm>
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "MoveGenerator.h"
#include "Parser.h"
#include "MoveBoard.h"
#include "Move.h"
#include "Square.h"

using namespace std;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DoctorBTest {
	TEST_CLASS(TestMoveGenerator) {
public:
	TEST_CLASS_INITIALIZE(setup) {
		MoveBoard::GetInstance().Initialize();
	}

	TEST_METHOD(TestMoveGeneratorGenerateKingMoves) {
		//arrange
		vector<string> tokens = {"8/8/8/3K4/8/8/8/8", "w"};
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);

		//act
		moveGen.GenerateKingMoves(moves);

		//assert
		Assert::AreEqual(8Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(35Ui8), Square(26Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(35Ui8), Square(27Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(35Ui8), Square(28Ui8))) != moves.end());

		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(35Ui8), Square(34Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(35Ui8), Square(36Ui8))) != moves.end());

		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(35Ui8), Square(42Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(35Ui8), Square(43Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(35Ui8), Square(44Ui8))) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateKnightMoves) {
		//arrange
		vector<string> tokens = { "8/8/8/3N4/8/8/8/8", "w" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);

		//act
		moveGen.GenerateKnightMoves(moves);

		//assert
		Assert::AreEqual(8Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(35Ui8), Square(18Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(35Ui8), Square(20Ui8))) != moves.end());
		
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(35Ui8), Square(25Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(35Ui8), Square(29Ui8))) != moves.end());

		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(35Ui8), Square(41Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(35Ui8), Square(45Ui8))) != moves.end());

		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(35Ui8), Square(50Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(35Ui8), Square(52Ui8))) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateWhitePawnPushes) {
		//arrange
		vector<string> tokens = { "8/8/8/8/8/8/3P4/8", "w" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);

		//act
		moveGen.GeneratePawnMoves(moves);

		//assert
		Assert::AreEqual(2Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(11Ui8), Square(19Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(11Ui8), Square(27Ui8))) != moves.end());
	}


	TEST_METHOD(TestMoveGeneratorGenerateWhitePawnCaptures) {
		//arrange
		vector<string> tokens = { "8/8/8/8/8/2nBp3/3P4/8", "w" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);

		//act
		moveGen.GeneratePawnMoves(moves);

		//assert
		Assert::AreEqual(2Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(11Ui8), Square(18Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(11Ui8), Square(20Ui8))) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateBlackPawnPushes) {
		//arrange
		vector<string> tokens = { "8/7p/8/8/8/8/8/8", "b" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);

		//act
		moveGen.GeneratePawnMoves(moves);

		//assert
		Assert::AreEqual(2Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(55Ui8), Square(47Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(55Ui8), Square(39Ui8))) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateBlackPawnCaptures) {
		//arrange
		vector<string> tokens = { "8/7p/6Rq/8/8/8/8/8", "b" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);

		//act
		moveGen.GeneratePawnMoves(moves);

		//assert
		Assert::AreEqual(1Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(55Ui8), Square(46Ui8))) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateRookMoves) {
		//arrange
		vector<string> tokens = { "8/8/8/8/1R6/8/8/8", "w" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);

		//act
		moveGen.GenerateRookMoves(moves);

		//assert
		Assert::AreEqual(14Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(25Ui8), Square(1Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(25Ui8), Square(9Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(25Ui8), Square(17Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(25Ui8), Square(33Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(25Ui8), Square(41i8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(25Ui8), Square(49i8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(25Ui8), Square(57i8))) != moves.end());

		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(25Ui8), Square(24i8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(25Ui8), Square(26i8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(25Ui8), Square(27i8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(25Ui8), Square(28i8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(25Ui8), Square(29i8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(25Ui8), Square(30i8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(25Ui8), Square(31i8))) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateRookMoves2) {
		//arrange
		vector<string> tokens = { "8/1q6/8/8/1RN5/1P6/8/8", "w" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);

		//act
		moveGen.GenerateRookMoves(moves);

		//assert
		Assert::AreEqual(4Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(25Ui8), Square(24Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(25Ui8), Square(33Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(25Ui8), Square(41Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(25Ui8), Square(49Ui8))) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateBishopMoves) {
		//arrange
		vector<string> tokens = { "8/4b3/8/8/8/8/8/8", "b" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);

		//act
		moveGen.GenerateBishopMoves(moves);

		//assert
		Assert::AreEqual(9Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(52Ui8), Square(59Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(52Ui8), Square(45Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(52Ui8), Square(38Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(52Ui8), Square(31Ui8))) != moves.end());
		
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(52Ui8), Square(16Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(52Ui8), Square(25Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(52Ui8), Square(34Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(52Ui8), Square(43Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(52Ui8), Square(61Ui8))) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateBishopMoves2) {
		//arrange
		vector<string> tokens = { "3k4/4b3/3p4/6R1/8/8/8/8", "b" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);

		//act
		moveGen.GenerateBishopMoves(moves);

		//assert
		Assert::AreEqual(3Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(52Ui8), Square(45Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(52Ui8), Square(38Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(52Ui8), Square(61Ui8))) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateQueenMoves) {
		//arrange
		vector<string> tokens = { "8/8/8/8/5Q2/8/8/8", "w" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);

		//act
		moveGen.GenerateQueenMoves(moves);

		//assert
		Assert::AreEqual(25Ui64, moves.size());
	}


	TEST_METHOD(TestMoveGeneratorGenerateQueenMoves2) {
		//arrange
		vector<string> tokens = { "8/8/3n1n2/8/4PQR1/8/8/8", "w" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);

		//act
		moveGen.GenerateQueenMoves(moves);

		//assert
		Assert::AreEqual(14Ui64, moves.size());
	}

	TEST_METHOD(TestMoveGeneratorStartPos) {
		//arrange
		Position pos;
		Parser::ParsePosition({ "startpos" }, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);

		//act
		moveGen.GenerateMoves(moves);

		//assert
		Assert::AreEqual(20Ui64, moves.size());
	}


	};
}