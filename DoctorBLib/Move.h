#pragma once
#include "Square.h"
#include "Piece.h"

class Move
{
public:
	Move();
	Move(const Piece piece, const Square square_from, const Square square_to);
	Move(const Piece piece, const Square square_from, const Square square_to, const Piece promo_piece);
	Move(const Square square_from, const Square square_to);
	Move(const Square square_from, const Square square_to, const Piece promo_piece);
	~Move();
	Square getSquareFrom() const;
	Square getSquareTo() const;
	bool getPromoPiece(Piece& promo_piece) const;
	void getPromoOrMovingPieceType(Piece& piece) const;
	bool operator==(const Move& that) const;
	bool isPromotion() const;
	bool isEpCapture() const;
	Move& setEpCapture();
	bool isCapture() const;
	Move& setCapture(bool capture);
	bool isCastling() const;
	Move& setCastling();
	bool isDoublePush() const;
	Move& setDoublePush();
	std::string toString() const;
	Move& setPiece(const Piece piece);
	void getPiece(Piece& piece) const;
	bool isValid() const;

private:
	static const uint8_t MASK_CAPTURE = 0b00000001Ui8;
	static const uint8_t MASK_PROMOTION = 0b00000010Ui8;
	static const uint8_t MASK_EP_CAPTURE = 0b00000100Ui8;
	static const uint8_t MASK_CASTLING = 0b00001000Ui8;
	static const uint8_t MASK_DOUBLE_PUSH = 0b00010000Ui8;
	
	Square square_from_;
	Square square_to_;
	uint8_t status_bits_;
	Piece piece_;
	Piece promo_piece_;
};

