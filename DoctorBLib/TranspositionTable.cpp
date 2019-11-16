#include "stdafx.h"
#include "TranspositionTable.h"
#include "Options.h"

using namespace std;

bool TranspositionTable::SetBestMove(const uint64_t key, const Move move) {
	auto it = map->find(key);
	if (it == map->end()) {
		if (map->size() >= max_size)
			return false;

		Entry entry;
		entry.best_move = move;
		map->insert(pair<uint64_t, Entry>(key, entry));
		return true;
	}

	map->at(key).best_move = move;
	return true;
}

bool TranspositionTable::SetScore(const uint64_t key, const Score score, uint16_t remaining_depth) {
	auto it = map->find(key);
	if (it == map->end()) {
		if (map->size() >= max_size)
			return false;

		Entry entry;
		entry.score = score;
		entry.remaining_depth = remaining_depth;
		map->insert(pair<uint64_t, Entry>(key, entry));
		return true;
	}

	map->at(key).score = score;
	map->at(key).remaining_depth = remaining_depth;
	return true;
}

bool TranspositionTable::FindBestMove(const uint64_t key, Move& move) {
	auto it = map->find(key);
	if (it == map->end())
		return false;
	
	if (!it->second.best_move.has_value())
		return false;

	move = it->second.best_move.value();
	return true;
}

bool TranspositionTable::FindScore(const uint64_t key, Score& score, uint16_t& remaining_depth) {
	auto it = map->find(key);
	if (it == map->end())
		return false;

	if (!it->second.score.has_value())
		return false;

	score = it->second.score.value();
	remaining_depth = it->second.remaining_depth;
	return true;
}

int TranspositionTable::GetHashFull() {
	return (int)(map->size() * 1000000 / max_size);
}

void TranspositionTable::Clear() {
	map->clear();
}

void TranspositionTable::Reset() {
	int elem_size = sizeof(pair<uint64_t, Entry>);
	max_size = (Options::GetInstance().MaxHashSizeMb * 1048576Ui32) / elem_size;
	map->clear();
	map->reserve(max_size);
}

TranspositionTable::TranspositionTable() {
	Reset();
}

TranspositionTable::~TranspositionTable() {
}

TranspositionTable& TranspositionTable::GetInstance() {
	static TranspositionTable instance;
	return instance;
}
