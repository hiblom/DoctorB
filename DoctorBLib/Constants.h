#pragma once
#include <string>

namespace Constants {
	const std::string PIECE_CHARS = "pPnNbBrRqQkK";

	const int32_t CASTLING_WHITE_KINGSIDE = 0;
	const int32_t CASTLING_WHITE_QUEENSIDE = 1;
	const int32_t CASTLING_BLACK_KINGSIDE = 2;
	const int32_t CASTLING_BLACK_QUEENSIDE = 3;

	const int32_t PIECE_VALUES[12] = { -100, 100, -300, 300, -300, 300, -500, 500, -900, 900, 0, 0 };
}