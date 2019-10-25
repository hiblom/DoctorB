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
	std::string ToString() const;
private:
	Square square_from;
	Square square_to;
	boost::optional<Piece> promo_piece;
};

