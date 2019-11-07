#pragma once
#include <cstdint>

class Zobrist
{
public:
	uint64_t SQUARE_PIECE_KEY[12][64];
	uint64_t BLACK_TURN_KEY;
	uint64_t CASTLING_KEY[4];
	uint64_t EP_FILE_KEY[8];

	~Zobrist();
	static Zobrist& GetInstance();
	Zobrist(const Zobrist&) = delete;
	void operator=(const Zobrist&) = delete;
private:
	const uint32_t SEED = 3775738323Ui32;
	Zobrist();
	void Initialize();
};

