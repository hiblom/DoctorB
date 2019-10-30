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
		vector<string> tokens = {"4k3/8/8/3K4/8/8/8/8", "w"};
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_KING, Piece::COLOR_WHITE);

		//act
		moveGen.GenerateKingMoves(moves);

		//assert
		Assert::AreEqual(8Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(35Ui8), Square(26Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(35Ui8), Square(27Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(35Ui8), Square(28Ui8))) != moves.end());

		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(35Ui8), Square(34Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(35Ui8), Square(36Ui8))) != moves.end());

		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(35Ui8), Square(42Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(35Ui8), Square(43Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(35Ui8), Square(44Ui8))) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorWhiteCastling) {
		//arrange
		vector<string> tokens = { "4k3/8/8/8/8/8/8/Q3K2R", "w", "KQkq"};
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_KING, Piece::COLOR_WHITE);

		//act
		moveGen.GenerateKingMoves(moves);

		//assert
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E1), Square(Square::G1)).SetCastling()) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E1), Square(Square::C1)).SetCastling()) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorBlackCastling) {
		//arrange
		vector<string> tokens = { "r3k2r/8/8/8/8/8/8/4K3", "b", "KQkq"};
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_KING, Piece::COLOR_BLACK);

		//act
		moveGen.GenerateKingMoves(moves);

		//assert
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E8), Square(Square::G8)).SetCastling()) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E8), Square(Square::C8)).SetCastling()) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorCannotCastle1) {
		//white cannot castle queenside because of lacking queenside castling rights

		//arrange
		vector<string> tokens = { "4k3/8/8/8/8/8/8/Q3K2R", "w", "K" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_KING, Piece::COLOR_WHITE);

		//act
		moveGen.GenerateKingMoves(moves);

		//assert
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E1), Square(Square::G1)).SetCastling()) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E1), Square(Square::C1)).SetCastling()) == moves.end());
	}

	TEST_METHOD(TestMoveGeneratorCannotCastle2) {
		//white cannot castle queenside because there is a piece in between
		//arrange
		vector<string> tokens = { "4k3/8/8/8/8/8/8/RN2K2R", "w", "KQkq" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_KING, Piece::COLOR_WHITE);

		//act
		moveGen.GenerateKingMoves(moves);

		//assert
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E1), Square(Square::G1)).SetCastling()) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E1), Square(Square::C1)).SetCastling()) == moves.end());
	}

	TEST_METHOD(TestMoveGeneratorCannotCastle3) {
		//white cannot castle at all because of being in check
		//arrange
		vector<string> tokens = { "4k3/8/8/8/1b6/8/8/R3K2R", "w", "KQkq" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_KING, Piece::COLOR_WHITE);

		//act
		moveGen.GenerateKingMoves(moves);

		//assert
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E1), Square(Square::G1)).SetCastling()) == moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E1), Square(Square::C1)).SetCastling()) == moves.end());
	}

	TEST_METHOD(TestMoveGeneratorCannotCastle4) {
		//white cannot castle queenside because one of the squares the king passes through is attacked
		//arrange
		vector<string> tokens = { "4k3/8/8/8/6b1/8/8/R3K2R", "w", "KQkq" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_KING, Piece::COLOR_WHITE);

		//act
		moveGen.GenerateKingMoves(moves);

		//assert
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E1), Square(Square::G1)).SetCastling()) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E1), Square(Square::C1)).SetCastling()) == moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateKnightMoves) {
		//arrange
		vector<string> tokens = { "4k3/8/8/3N4/8/8/8/4K3", "w" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_KNIGHT, Piece::COLOR_WHITE);

		//act
		moveGen.GenerateKnightMoves(moves);

		//assert
		Assert::AreEqual(8Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(35Ui8), Square(18Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(35Ui8), Square(20Ui8))) != moves.end());
		
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(35Ui8), Square(25Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(35Ui8), Square(29Ui8))) != moves.end());

		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(35Ui8), Square(41Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(35Ui8), Square(45Ui8))) != moves.end());

		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(35Ui8), Square(50Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(35Ui8), Square(52Ui8))) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateWhitePawnPushes) {
		//arrange
		vector<string> tokens = { "4k3/8/8/8/8/8/3P4/4K3", "w" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);

		//act
		moveGen.GeneratePawnMoves(moves);

		//assert
		Assert::AreEqual(2Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(11Ui8), Square(19Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(Square(11Ui8), Square(27Ui8)).SetDoublePush()) != moves.end());
	}


	TEST_METHOD(TestMoveGeneratorGenerateWhitePawnCaptures) {
		//arrange
		vector<string> tokens = { "4k3/8/8/8/8/2nBp3/3P4/4K3", "w" };
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
		vector<string> tokens = { "4k3/7p/8/8/8/8/8/4K3", "b" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK);

		//act
		moveGen.GeneratePawnMoves(moves);

		//assert
		Assert::AreEqual(2Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(55Ui8), Square(47Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(55Ui8), Square(39Ui8)).SetDoublePush()) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateBlackPawnCaptures) {
		//arrange
		vector<string> tokens = { "4k3/7p/6Rq/8/8/8/8/4K3", "b" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK);

		//act
		moveGen.GeneratePawnMoves(moves);

		//assert
		Assert::AreEqual(1Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(55Ui8), Square(46Ui8))) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGeneratePromotions) {
		//arrange
		vector<string> tokens = { "2k1n3/5P2/8/8/8/8/8/4K3", "w" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE);

		//act
		moveGen.GeneratePawnMoves(moves);

		//assert
		Assert::AreEqual(8Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(53Ui8), Square(61Ui8), Piece(Piece::TYPE_QUEEN, Piece::COLOR_WHITE))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(53Ui8), Square(61Ui8), Piece(Piece::TYPE_KNIGHT, Piece::COLOR_WHITE))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(53Ui8), Square(61Ui8), Piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(53Ui8), Square(61Ui8), Piece(Piece::TYPE_BISHOP, Piece::COLOR_WHITE))) != moves.end());

		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(53Ui8), Square(60Ui8), Piece(Piece::TYPE_QUEEN, Piece::COLOR_WHITE))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(53Ui8), Square(60Ui8), Piece(Piece::TYPE_KNIGHT, Piece::COLOR_WHITE))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(53Ui8), Square(60Ui8), Piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(53Ui8), Square(60Ui8), Piece(Piece::TYPE_BISHOP, Piece::COLOR_WHITE))) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateEpCapture) {
		//arrange
		vector<string> tokens = { "4k3/8/8/8/2Pp4/8/8/4K3", "b", "-", "c3" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK);

		//act
		moveGen.GeneratePawnMoves(moves);

		//assert
		Assert::AreEqual(2Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(27Ui8), Square(19Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(27Ui8), Square(18Ui8)).SetEpCapture()) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateRookMoves) {
		//arrange
		vector<string> tokens = { "4k3/8/8/8/1R6/8/8/4K3", "w" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE);

		//act
		moveGen.GenerateRookMoves(moves);

		//assert
		Assert::AreEqual(14Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(25Ui8), Square(1Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(25Ui8), Square(9Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(25Ui8), Square(17Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(25Ui8), Square(33Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(25Ui8), Square(41i8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(25Ui8), Square(49i8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(25Ui8), Square(57i8))) != moves.end());

		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(25Ui8), Square(24i8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(25Ui8), Square(26i8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(25Ui8), Square(27i8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(25Ui8), Square(28i8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(25Ui8), Square(29i8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(25Ui8), Square(30i8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(25Ui8), Square(31i8))) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateRookMoves2) {
		//arrange
		vector<string> tokens = { "4k3/1q6/8/8/1RN5/1P6/8/4K3", "w" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE);

		//act
		moveGen.GenerateRookMoves(moves);

		//assert
		Assert::AreEqual(4Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(25Ui8), Square(24Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(25Ui8), Square(33Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(25Ui8), Square(41Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(25Ui8), Square(49Ui8))) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateBishopMoves) {
		//arrange
		vector<string> tokens = { "2k5/4b3/8/8/8/8/8/4K3", "b" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_BISHOP, Piece::COLOR_BLACK);

		//act
		moveGen.GenerateBishopMoves(moves);

		//assert
		Assert::AreEqual(9Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(52Ui8), Square(59Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(52Ui8), Square(45Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(52Ui8), Square(38Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(52Ui8), Square(31Ui8))) != moves.end());
		
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(52Ui8), Square(16Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(52Ui8), Square(25Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(52Ui8), Square(34Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(52Ui8), Square(43Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(52Ui8), Square(61Ui8))) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateBishopMoves2) {
		//arrange
		vector<string> tokens = { "3k4/4b3/3p4/6R1/8/8/8/4K3", "b" };
		Position pos;
		Parser::ParseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_BISHOP, Piece::COLOR_BLACK);

		//act
		moveGen.GenerateBishopMoves(moves);

		//assert
		Assert::AreEqual(3Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(52Ui8), Square(45Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(52Ui8), Square(38Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(52Ui8), Square(61Ui8))) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateQueenMoves) {
		//arrange
		vector<string> tokens = { "4k3/8/8/8/5Q2/8/8/4K3", "w" };
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
		vector<string> tokens = { "4k3/8/3n1n2/8/4PQR1/8/8/4K3", "w" };
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

	//TEST_METHOD(TestMoveGeneratorLegalKingMoves) {
	//	//arrange
	//	vector<string> tokens = { "8/5k2/8/5R2/8/8/8/4K3", "b" };
	//	Position pos;
	//	Parser::ParseFen(tokens, pos);
	//	vector<Move> moves;
	//	MoveGenerator moveGen = MoveGenerator(pos);
	//	Piece piece(Piece::TYPE_KING, Piece::COLOR_BLACK);

	//	//act
	//	moveGen.GenerateKingMoves(moves);

	//	//assert
	//	Assert::AreEqual(6Ui64, moves.size());
	//	Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::F7), Square(Square::E6))) != moves.end());
	//	Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::F7), Square(Square::E7))) != moves.end());
	//	Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::F7), Square(Square::E8))) != moves.end());

	//	Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::F7), Square(Square::G6))) != moves.end());
	//	Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::F7), Square(Square::G7))) != moves.end());
	//	Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::F7), Square(Square::G8))) != moves.end());
	//}

	//TEST_METHOD(TestMoveGeneratorCheckerCount) {
	//	//arrange
	//	vector<string> tokens = { "4k3/6N1/5b2/4R3/8/8/8/4K3", "b" };
	//	Position pos;
	//	Parser::ParseFen(tokens, pos);

	//	//act
	//	MoveGenerator moveGen = MoveGenerator(pos);

	//	//assert
	//	Assert::AreEqual(2Ui8, moveGen.GetCheckerCount());
	//}

	//TEST_METHOD(TestMoveGeneratorCheckEvasion1) {
	//	//arrange
	//	vector<string> tokens = { "4k3/8/6n1/4R3/8/8/8/4K3", "b" };
	//	Position pos;
	//	Parser::ParseFen(tokens, pos);
	//	MoveGenerator moveGen = MoveGenerator(pos);
	//	vector<Move> moves;
	//	Piece king = Piece(Piece::TYPE_KING, Piece::COLOR_BLACK);
	//	Piece horse = Piece(Piece::TYPE_KNIGHT, Piece::COLOR_BLACK);

	//	//act
	//	moveGen.GenerateMoves(moves);

	//	//assert
	//	Assert::AreEqual(6Ui64, moves.size());

	//	//king moves x 4
	//	Assert::IsTrue(find(moves.begin(), moves.end(), Move(king, Square(Square::E8), Square(Square::D8))) != moves.end());
	//	Assert::IsTrue(find(moves.begin(), moves.end(), Move(king, Square(Square::E8), Square(Square::D7))) != moves.end());
	//	Assert::IsTrue(find(moves.begin(), moves.end(), Move(king, Square(Square::E8), Square(Square::F8))) != moves.end());
	//	Assert::IsTrue(find(moves.begin(), moves.end(), Move(king, Square(Square::E8), Square(Square::F7))) != moves.end());

	//	//block by knight
	//	Assert::IsTrue(find(moves.begin(), moves.end(), Move(horse, Square(Square::G6), Square(Square::E7))) != moves.end());

	//	//capture rook by knight
	//	Assert::IsTrue(find(moves.begin(), moves.end(), Move(horse, Square(Square::G6), Square(Square::E5))) != moves.end());
	//}



	};
}