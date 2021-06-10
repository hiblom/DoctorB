#pragma once

#include <cstdint>
#include <fstream>
#include <memory>
#include "Move.h"

struct PolyglotEntry {
	uint64_t hash;
	uint16_t move;
	uint16_t weight;
	uint32_t learn;
};

class Polyglot
{
public:
	static Polyglot& getInstance();
	bool open();
	Move getMove(uint64_t hash);
	Polyglot(Polyglot& other) = delete;
	void operator=(const Polyglot&) = delete;
	~Polyglot();
private:
	Polyglot();
	void readEntry(PolyglotEntry& entry);
	Move convertMove(uint16_t polyglot_move);
	int getRandom(int max);
	bool binarySearch(uint64_t hash);

	static std::unique_ptr<Polyglot> instance_;
	std::string filePath_ = "";
	std::ifstream file_;
};