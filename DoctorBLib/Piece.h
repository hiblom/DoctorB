#pragma once
#include <cstdint>
#include <string>

class Piece
{
public:
	static const std::uint8_t TYPE_PAWN = 0;
	static const std::uint8_t TYPE_KING = 2;
	static const std::uint8_t TYPE_QUEEN = 4;
	static const std::uint8_t TYPE_ROOK = 6;
	static const std::uint8_t TYPE_BISHOP = 8;
	static const std::uint8_t TYPE_KNIGHT = 10;
	static const std::uint8_t COLOR_WHITE = 0;
	static const std::uint8_t COLOR_BLACK = 1;

	Piece();
	Piece(uint8_t piece_type, uint8_t piece_color);
	~Piece();
	uint8_t GetType() const;
	void SetType(uint8_t piece_type);
	uint8_t GetColor() const;
	void SetColor(uint8_t piece_color);
	uint8_t GetValue() const;
	void SetValue(uint8_t piece_value);
	char ToChar() const;
	std::string ToString() const;
	bool operator==(const Piece& that) const;
private:
	static const std::uint8_t MASK_COLOR = 0b00000001;
	static const std::uint8_t MASK_TYPE = 0b11111110;
	uint8_t value;
};

