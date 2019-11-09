#include "stdafx.h"
#include "TranspositionTable.h"
#include "Options.h"

using namespace std;

bool TranspositionTable::AddEntry(const uint64_t key, const Entry entry) {
	auto it = map.find(key);
	if (it == map.end()) {
		if (map.size() == max_size)
			return false;

		map.insert(pair<uint64_t, Entry>(key, entry));
		return true;
	}
	
	map.at(key) = entry;
	return true;
}

bool TranspositionTable::FindEntry(const uint64_t key, Entry& entry) {
	auto it = map.find(key);
	if (it == map.end())
		return false;
	entry = it->second;
	return true;
}

void TranspositionTable::Clear() {
	map.clear();
}


void TranspositionTable::Reset() {
	int elem_size = sizeof(pair<uint64_t, Entry>);
	max_size = (Options::GetInstance().MaxHashSizeMb * 1048576Ui32) / elem_size;
	map.clear();
	map.reserve(max_size);
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
