#pragma once
#include "boost/optional.hpp"
#include "Square.h"
#include "Piece.h"

class Move
{
public:
	Move();
	~Move();
	Move(const Square move_square_from, const Square move_square_to);
	Move(const Square move_square_from, const Square move_square_to, const Piece move_promo_piece);
	Square GetSquareFrom() const;
	Square GetSquareTo() const;
	bool GetPromoPiece(Piece& piece) const;
	bool operator==(const Move& that) const;
	//bool IsCapture() const;
	//Move& SetCapture();
	bool IsPromotion() const;
	bool IsEpCapture() const;
	Move& SetEpCapture();
	bool IsCastling() const;
	Move& SetCastling();
	bool IsDoublePush() const;
	Move& SetDoublePush();
	std::string ToString() const;
private:
	//static const uint16_t MASK_CAPTURE = 0b0000000000000001Ui16;
	static const uint16_t MASK_PROMOTION = 0b0000000000000010Ui16;
	static const uint16_t MASK_EP_CAPTURE = 0b0000000000000100Ui16;
	static const uint16_t MASK_CASTLING = 0b0000000000001000Ui16;
	static const uint16_t MASK_DOUBLE_PUSH = 0b0000000000010000Ui16;
	static const uint16_t MASK_PROMO_PIECE = 0b1111111100000000Ui16;

	Square square_from;
	Square square_to;
	uint16_t status;
};

