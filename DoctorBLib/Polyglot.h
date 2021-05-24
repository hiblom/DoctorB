#pragma once

#include <cstdint>
#include <fstream>
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
	static Polyglot& get_instance();
	bool open();
	Move get_move(uint64_t hash);
	Polyglot(Polyglot& other) = delete;
	void operator=(const Polyglot&) = delete;
private:
	Polyglot();
	~Polyglot();
	void read_entry(PolyglotEntry& entry);
	Move convert_move(uint16_t polyglot_move);
	int get_random(int max);
	static Polyglot* instance_;
	std::string filePath_ = "";
	std::ifstream file_;
};