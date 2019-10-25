#pragma once
#include <cstdint>
#include <string>

class Square
{
public:
	static const uint8_t A1 =  0Ui8;
	static const uint8_t A2 =  8Ui8;
	static const uint8_t A3 = 16Ui8;
	static const uint8_t A4 = 24Ui8;
	static const uint8_t A5 = 32Ui8;
	static const uint8_t A6 = 40Ui8;
	static const uint8_t A7 = 48Ui8;
	static const uint8_t A8 = 56Ui8;

	static const uint8_t B1 =  1Ui8;
	static const uint8_t B2 =  9Ui8;
	static const uint8_t B3 = 17Ui8;
	static const uint8_t B4 = 25Ui8;
	static const uint8_t B5 = 33Ui8;
	static const uint8_t B6 = 41Ui8;
	static const uint8_t B7 = 49Ui8;
	static const uint8_t B8 = 57Ui8;

	static const uint8_t C1 =  2Ui8;
	static const uint8_t C2 = 10Ui8;
	static const uint8_t C3 = 18Ui8;
	static const uint8_t C4 = 26Ui8;
	static const uint8_t C5 = 34Ui8;
	static const uint8_t C6 = 42Ui8;
	static const uint8_t C7 = 50Ui8;
	static const uint8_t C8 = 58Ui8;

	static const uint8_t D1 =  3Ui8;
	static const uint8_t D2 = 11Ui8;
	static const uint8_t D3 = 19Ui8;
	static const uint8_t D4 = 27Ui8;
	static const uint8_t D5 = 35Ui8;
	static const uint8_t D6 = 43Ui8;
	static const uint8_t D7 = 51Ui8;
	static const uint8_t D8 = 59Ui8;

	static const uint8_t E1 =  4Ui8;
	static const uint8_t E2 = 12Ui8;
	static const uint8_t E3 = 20Ui8;
	static const uint8_t E4 = 28Ui8;
	static const uint8_t E5 = 36Ui8;
	static const uint8_t E6 = 44Ui8;
	static const uint8_t E7 = 52Ui8;
	static const uint8_t E8 = 60Ui8;

	static const uint8_t F1 =  5Ui8;
	static const uint8_t F2 = 13Ui8;
	static const uint8_t F3 = 21Ui8;
	static const uint8_t F4 = 29Ui8;
	static const uint8_t F5 = 37Ui8;
	static const uint8_t F6 = 45Ui8;
	static const uint8_t F7 = 53Ui8;
	static const uint8_t F8 = 61Ui8;

	static const uint8_t G1 =  6Ui8;
	static const uint8_t G2 = 14Ui8;
	static const uint8_t G3 = 22Ui8;
	static const uint8_t G4 = 30Ui8;
	static const uint8_t G5 = 38Ui8;
	static const uint8_t G6 = 46Ui8;
	static const uint8_t G7 = 54Ui8;
	static const uint8_t G8 = 62Ui8;

	static const uint8_t H1 =  7Ui8;
	static const uint8_t H2 = 15Ui8;
	static const uint8_t H3 = 23Ui8;
	static const uint8_t H4 = 31Ui8;
	static const uint8_t H5 = 39Ui8;
	static const uint8_t H6 = 47Ui8;
	static const uint8_t H7 = 55Ui8;
	static const uint8_t H8 = 63Ui8;

	Square();
	Square(uint8_t square_value);
	Square(uint8_t x, uint8_t y);
	~Square();
	uint8_t GetValue() const;
	void SetValue(uint8_t square_value);
	void SetXy(uint8_t x, uint8_t y);
	uint8_t GetX() const;
	uint8_t GetY() const;
	bool operator==(const Square& that) const;
	std::string ToString() const;
private:
	static const uint8_t MASK_X = 0b00000111Ui8;
	uint8_t value;
};

