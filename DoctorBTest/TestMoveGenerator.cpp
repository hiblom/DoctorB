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
	TEST_METHOD(TestMoveGeneratorGenerateKingMoves) {
		//arrange
		vector<string> tokens = {"4k3/8/8/3K4/8/8/8/8", "w"};
		Position pos;
		Parser::parseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_KING, Piece::COLOR_WHITE);

		//act
		moveGen.generateKingMoves(moves);

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
		Parser::parseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_KING, Piece::COLOR_WHITE);

		//act
		moveGen.generateKingMoves(moves);

		//assert
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E1), Square(Square::G1)).setCastling()) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E1), Square(Square::C1)).setCastling()) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorBlackCastling) {
		//arrange
		vector<string> tokens = { "r3k2r/8/8/8/8/8/8/4K3", "b", "KQkq"};
		Position pos;
		Parser::parseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_KING, Piece::COLOR_BLACK);

		//act
		moveGen.generateKingMoves(moves);

		//assert
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E8), Square(Square::G8)).setCastling()) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E8), Square(Square::C8)).setCastling()) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorCannotCastle1) {
		//white cannot castle queenside because of lacking queenside castling rights

		//arrange
		vector<string> tokens = { "4k3/8/8/8/8/8/8/Q3K2R", "w", "K" };
		Position pos;
		Parser::parseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_KING, Piece::COLOR_WHITE);

		//act
		moveGen.generateKingMoves(moves);

		//assert
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E1), Square(Square::G1)).setCastling()) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E1), Square(Square::C1)).setCastling()) == moves.end());
	}

	TEST_METHOD(TestMoveGeneratorCannotCastle2) {
		//white cannot castle queenside because there is a piece in between
		//arrange
		vector<string> tokens = { "4k3/8/8/8/8/8/8/RN2K2R", "w", "KQkq" };
		Position pos;
		Parser::parseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_KING, Piece::COLOR_WHITE);

		//act
		moveGen.generateKingMoves(moves);

		//assert
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E1), Square(Square::G1)).setCastling()) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E1), Square(Square::C1)).setCastling()) == moves.end());
	}

	TEST_METHOD(TestMoveGeneratorCannotCastle3) {
		//white cannot castle at all because of being in check
		//arrange
		vector<string> tokens = { "4k3/8/8/8/1b6/8/8/R3K2R", "w", "KQkq" };
		Position pos;
		Parser::parseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_KING, Piece::COLOR_WHITE);

		//act
		moveGen.generateKingMoves(moves);

		//assert
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E1), Square(Square::G1)).setCastling()) == moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E1), Square(Square::C1)).setCastling()) == moves.end());
	}

	TEST_METHOD(TestMoveGeneratorCannotCastle4) {
		//white cannot castle queenside because one of the squares the king passes through is attacked
		//arrange
		vector<string> tokens = { "4k3/8/8/8/6b1/8/8/R3K2R", "w", "KQkq" };
		Position pos;
		Parser::parseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_KING, Piece::COLOR_WHITE);

		//act
		moveGen.generateKingMoves(moves);

		//assert
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E1), Square(Square::G1)).setCastling()) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E1), Square(Square::C1)).setCastling()) == moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateKnightMoves) {
		//arrange
		vector<string> tokens = { "4k3/8/8/3N4/8/8/8/4K3", "w" };
		Position pos;
		Parser::parseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_KNIGHT, Piece::COLOR_WHITE);

		//act
		moveGen.generateKnightMoves(moves);

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
		Parser::parseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE);

		//act
		moveGen.generatePawnMoves(moves);

		//assert
		Assert::AreEqual(2Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(11Ui8), Square(19Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(11Ui8), Square(27Ui8)).setDoublePush()) != moves.end());
	}


	TEST_METHOD(TestMoveGeneratorGenerateWhitePawnCaptures) {
		//arrange
		vector<string> tokens = { "4k3/8/8/8/8/2nBp3/3P4/4K3", "w" };
		Position pos;
		Parser::parseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE);

		//act
		moveGen.generatePawnMoves(moves);

		//assert
		Assert::AreEqual(2Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::D2), Square(Square::C3)).setCapture(true)) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::D2), Square(Square::E3)).setCapture(true)) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateBlackPawnPushes) {
		//arrange
		vector<string> tokens = { "4k3/7p/8/8/8/8/8/4K3", "b" };
		Position pos;
		Parser::parseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK);

		//act
		moveGen.generatePawnMoves(moves);

		//assert
		Assert::AreEqual(2Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(55Ui8), Square(47Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(55Ui8), Square(39Ui8)).setDoublePush()) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateBlackPawnCaptures) {
		//arrange
		vector<string> tokens = { "4k3/7p/6Rq/8/8/8/8/4K3", "b" };
		Position pos;
		Parser::parseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK);

		//act
		moveGen.generatePawnMoves(moves);

		//assert
		Assert::AreEqual(1Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(55Ui8), Square(46Ui8)).setCapture(true)) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGeneratePromotions) {
		//arrange
		vector<string> tokens = { "2k1n3/5P2/8/8/8/8/8/4K3", "w" };
		Position pos;
		Parser::parseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_PAWN, Piece::COLOR_WHITE);

		//act
		moveGen.generatePawnMoves(moves);

		//assert
		Assert::AreEqual(8Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::F7), Square(Square::F8), Piece(Piece::TYPE_QUEEN, Piece::COLOR_WHITE))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::F7), Square(Square::F8), Piece(Piece::TYPE_KNIGHT, Piece::COLOR_WHITE))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::F7), Square(Square::F8), Piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::F7), Square(Square::F8), Piece(Piece::TYPE_BISHOP, Piece::COLOR_WHITE))) != moves.end());

		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::F7), Square(Square::E8), Piece(Piece::TYPE_QUEEN, Piece::COLOR_WHITE)).setCapture(true)) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::F7), Square(Square::E8), Piece(Piece::TYPE_KNIGHT, Piece::COLOR_WHITE)).setCapture(true)) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::F7), Square(Square::E8), Piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE)).setCapture(true)) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::F7), Square(Square::E8), Piece(Piece::TYPE_BISHOP, Piece::COLOR_WHITE)).setCapture(true)) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateEpCapture) {
		//arrange
		vector<string> tokens = { "4k3/8/8/8/2Pp4/8/8/4K3", "b", "-", "c3" };
		Position pos;
		Parser::parseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK);

		//act
		moveGen.generatePawnMoves(moves);

		//assert
		Assert::AreEqual(2Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(27Ui8), Square(19Ui8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(27Ui8), Square(18Ui8)).setEpCapture()) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateRookMoves) {
		//arrange
		vector<string> tokens = { "4k3/8/8/8/1R6/8/8/4K3", "w" };
		Position pos;
		Parser::parseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE);

		//act
		moveGen.generateRookMoves(moves);

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
		Parser::parseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE);

		//act
		moveGen.generateRookMoves(moves);

		//assert
		Assert::AreEqual(4Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::B4), Square(Square::A4))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::B4), Square(Square::B5))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::B4), Square(Square::B6))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::B4), Square(Square::B7)).setCapture(true)) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateBishopMoves) {
		//arrange
		vector<string> tokens = { "2k5/4b3/8/8/8/8/8/4K3", "b" };
		Position pos;
		Parser::parseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_BISHOP, Piece::COLOR_BLACK);

		//act
		moveGen.generateBishopMoves(moves);

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
		Parser::parseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_BISHOP, Piece::COLOR_BLACK);

		//act
		moveGen.generateBishopMoves(moves);

		//assert
		Assert::AreEqual(3Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E7), Square(Square::F8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E7), Square(Square::F6))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::E7), Square(Square::G5)).setCapture(true)) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorGenerateQueenMoves) {
		//arrange
		vector<string> tokens = { "4k3/8/8/8/5Q2/8/8/4K3", "w" };
		Position pos;
		Parser::parseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);

		//act
		moveGen.generateQueenMoves(moves);

		//assert
		Assert::AreEqual(25Ui64, moves.size());
	}


	TEST_METHOD(TestMoveGeneratorGenerateQueenMoves2) {
		//arrange
		vector<string> tokens = { "4k3/8/3n1n2/8/4PQR1/8/8/4K3", "w" };
		Position pos;
		Parser::parseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);

		//act
		moveGen.generateQueenMoves(moves);

		//assert
		Assert::AreEqual(14Ui64, moves.size());
	}

	TEST_METHOD(TestMoveGeneratorStartPos) {
		//arrange
		Position pos;
		HistoryMap history;
		Parser::parsePosition({ "startpos" }, pos, history);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);

		//act
		moveGen.generateMoves(moves);

		//assert
		Assert::AreEqual(20Ui64, moves.size());
	}

	TEST_METHOD(TestMoveGeneratorStartPosA2A4) {
		//arrange
		Position pos;
		HistoryMap history;
		vector<string> tokens = { "startpos", "moves", "a2a4" };
		Parser::parsePosition(tokens, pos, history);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);

		//act
		moveGen.generateMoves(moves);

		//assert
		Assert::AreEqual(20Ui64, moves.size());
	}

	TEST_METHOD(TestMoveGeneratorLegalKingMoves) {
		//arrange
		vector<string> tokens = { "8/5k2/8/5R2/8/8/8/4K3", "b" };
		Position pos;
		Parser::parseFen(tokens, pos);
		vector<Move> moves;
		MoveGenerator moveGen = MoveGenerator(pos);
		Piece piece(Piece::TYPE_KING, Piece::COLOR_BLACK);

		//act
		moveGen.generateKingMoves(moves);

		//assert
		Assert::AreEqual(6Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::F7), Square(Square::E6))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::F7), Square(Square::E7))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::F7), Square(Square::E8))) != moves.end());

		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::F7), Square(Square::G6))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::F7), Square(Square::G7))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(piece, Square(Square::F7), Square(Square::G8))) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorCheckerCount) {
		//arrange
		vector<string> tokens = { "4k3/6N1/5b2/4R3/8/8/8/4K3", "b" };
		Position pos;
		Parser::parseFen(tokens, pos);

		//act
		MoveGenerator moveGen = MoveGenerator(pos);

		//assert
		Assert::AreEqual(2Ui8, moveGen.getCheckerCount());
	}

	TEST_METHOD(TestMoveGeneratorCheckEvasion1) {
		//arrange
		vector<string> tokens = { "4k3/8/6n1/4R3/8/8/8/4K3", "b" };
		Position pos;
		Parser::parseFen(tokens, pos);
		MoveGenerator moveGen = MoveGenerator(pos);
		vector<Move> moves;
		Piece king = Piece(Piece::TYPE_KING, Piece::COLOR_BLACK);
		Piece horse = Piece(Piece::TYPE_KNIGHT, Piece::COLOR_BLACK);

		//act
		moveGen.generateMoves(moves);

		//assert
		Assert::AreEqual(6Ui64, moves.size());

		//king moves x 4
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(king, Square(Square::E8), Square(Square::D8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(king, Square(Square::E8), Square(Square::D7))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(king, Square(Square::E8), Square(Square::F8))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(king, Square(Square::E8), Square(Square::F7))) != moves.end());

		//block by knight
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(horse, Square(Square::G6), Square(Square::E7))) != moves.end());

		//capture rook by knight
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(horse, Square(Square::G6), Square(Square::E5)).setCapture(true)) != moves.end());
	}

	//8/8/8/2k5/3Pp3/8/8/4K3 w - - 0 2 

	TEST_METHOD(TestMoveGeneratorCheckEvasionEnPassant) {
		//arrange
		vector<string> tokens = { "8/8/8/2k5/3Pp3/8/8/4K3", "b", "", "d3" };
		Position pos;
		Parser::parseFen(tokens, pos);
		MoveGenerator moveGen = MoveGenerator(pos);
		vector<Move> moves;
		Piece pawn = Piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK);

		//act
		moveGen.generatePawnMoves(moves);

		//assert
		Assert::AreEqual(1Ui64, moves.size());

		//ep capture is possible
		Move expected = Move(pawn, Square(Square::E4), Square(Square::D3)).setEpCapture();
		Assert::IsTrue(find(moves.begin(), moves.end(), expected) != moves.end());

	}

	TEST_METHOD(TestMoveGeneratorPinnedPiece1) {
		//arrange
		vector<string> tokens = { "4k3/4r3/8/8/8/8/4R3/4K3", "b" };
		Position pos;
		Parser::parseFen(tokens, pos);
		MoveGenerator moveGen = MoveGenerator(pos);
		vector<Move> moves;
		Piece tower = Piece(Piece::TYPE_ROOK, Piece::COLOR_BLACK);

		//act
		moveGen.generateRookMoves(moves);

		//assert
		Assert::AreEqual(5Ui64, moves.size());

		//rook moves x 4 along pin ray
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(tower, Square(Square::E7), Square(Square::E6))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(tower, Square(Square::E7), Square(Square::E5))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(tower, Square(Square::E7), Square(Square::E4))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(tower, Square(Square::E7), Square(Square::E3))) != moves.end());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(tower, Square(Square::E7), Square(Square::E2)).setCapture(true)) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorPinnedPiece2) {
		//arrange
		vector<string> tokens = { "4k3/8/8/4p3/5P2/8/4R3/4K3", "b" };
		Position pos;
		Parser::parseFen(tokens, pos);
		MoveGenerator moveGen = MoveGenerator(pos);
		vector<Move> moves;
		Piece pawn = Piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK);

		//act
		moveGen.generatePawnMoves(moves);

		//assert
		Assert::AreEqual(1Ui64, moves.size());
		
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(pawn, Square(Square::E5), Square(Square::E4))) != moves.end());
	}

	TEST_METHOD(TestMoveGeneratorDiscoveredEnPassant) {
		//arrange
		vector<string> tokens = { "8/2p5/3p4/KP5r/1R2Pp1k/8/6P1/8", "b", "-", "e3" };
		Position pos;
		Parser::parseFen(tokens, pos);
		MoveGenerator moveGen = MoveGenerator(pos);
		vector<Move> moves;
		Piece pawn = Piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK);

		//act
		moveGen.generatePawnMoves(moves);

		//assert
		//plack pawn cannot capture E.P., because of discovered check from rook on b4
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(pawn, Square(Square::F4), Square(Square::E3)).setEpCapture()) == moves.end());
	}

	TEST_METHOD(TestMoveGeneratorCapturesOnly) {
		//rnbqkb1r/pppp1ppp/5n2/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3 
		//arrange
		vector<string> tokens = { "rnbqkb1r/pppp1ppp/5n2/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R", "w", "KQkq" };
		Position pos;
		Parser::parseFen(tokens, pos);
		MoveGenerator moveGen = MoveGenerator(pos, true);
		vector<Move> moves;
		Piece horse = Piece(Piece::TYPE_KNIGHT, Piece::COLOR_WHITE);

		//act
		moveGen.generateMoves(moves);

		//assert
		//only one capture available
		Assert::AreEqual(1Ui64, moves.size());
		Assert::IsTrue(find(moves.begin(), moves.end(), Move(horse, Square(Square::F3), Square(Square::E5)).setCapture(true)) != moves.end());

	}

	TEST_METHOD(TestMoveGeneratorLvaCapture) {
		//4kq2/6n1/4p3/5Q2/8/6N1/8/4K3 w
		//arrange
		vector<string> tokens = { "4kq2/6n1/4p3/5Q2/8/6N1/8/4K3", "b" };
		Position pos;
		Parser::parseFen(tokens, pos);
		MoveGenerator moveGen = MoveGenerator(pos, true);
		Move actual_capture;
		Move expected_capture = Move(Piece(Piece::TYPE_PAWN, Piece::COLOR_BLACK), Square(Square::E6), Square(Square::F5)).setCapture(true);

		//act
		bool result = moveGen.getLvaCapture(Square(Square::F5), actual_capture);

		//assert
		Assert::IsTrue(result);
		Assert::AreEqual(expected_capture, actual_capture);

	}

	TEST_METHOD(TestMoveGeneratorLvaCapturePinned) {
		//white knight is pinned
		//arrange
		vector<string> tokens = { "4k3/4q1n1/8/R4p2/8/4N3/8/4K3", "w" };
		Position pos;
		Parser::parseFen(tokens, pos);
		MoveGenerator moveGen = MoveGenerator(pos, true);
		Move actual_capture;
		Move expected_capture = Move(Piece(Piece::TYPE_ROOK, Piece::COLOR_WHITE), Square(Square::A5), Square(Square::F5)).setCapture(true);

		//act
		bool result = moveGen.getLvaCapture(Square(Square::F5), actual_capture);

		//assert
		Assert::IsTrue(result);
		Assert::AreEqual(expected_capture, actual_capture);

	}



	};
}