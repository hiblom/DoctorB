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
	Square GetSquareFrom() const;
	Square GetSquareTo() const;
	bool GetPromoPiece(Piece& promo_piece) const;
	void GetPromoOrMovingPieceType(Piece& piece) const;
	bool operator==(const Move& that) const;
	bool IsPromotion() const;
	bool IsEpCapture() const;
	Move& SetEpCapture();
	bool IsCapture() const;
	Move& SetCapture(bool capture);
	bool IsCastling() const;
	Move& SetCastling();
	bool IsDoublePush() const;
	Move& SetDoublePush();
	std::string ToString() const;
	Move& SetPiece(const Piece piece);
	void GetPiece(Piece& piece) const;
	bool IsValid() const;

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

