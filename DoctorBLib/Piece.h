#pragma once
#include <cstdint>
#include <string>

class Piece
{
public:
	static const std::uint8_t TYPE_PAWN = 0Ui8;
	static const std::uint8_t TYPE_KNIGHT = 2Ui8;
	static const std::uint8_t TYPE_BISHOP = 4Ui8;
	static const std::uint8_t TYPE_ROOK = 6Ui8;
	static const std::uint8_t TYPE_QUEEN = 8Ui8;
	static const std::uint8_t TYPE_KING = 10Ui8;
	static const std::uint8_t COLOR_BLACK = 0Ui8;
	static const std::uint8_t COLOR_WHITE = 1Ui8;

	Piece();
	Piece(uint8_t piece_value);
	Piece(uint8_t piece_type, uint8_t piece_color);
	~Piece();
	uint8_t getType() const;
	void setType(uint8_t piece_type);
	uint8_t getColor() const;
	void setColor(uint8_t piece_color);
	uint8_t getValue() const;
	void setValue(uint8_t piece_value);
	char toChar() const;
	std::string toString() const;
	bool operator==(const Piece& that) const;
private:
	static const std::uint8_t MASK_COLOR = 0b00000001Ui8;
	static const std::uint8_t MASK_TYPE = 0b11111110Ui8;
	uint8_t value_;
};

