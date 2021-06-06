#include "stdafx.h"
#include "HistoryMap.h"

using namespace std;

HistoryMap::HistoryMap() {
}

HistoryMap::~HistoryMap() {
}

void HistoryMap::increase(const uint64_t hash_key) {
	auto it = map_.find(hash_key);
	if (it == map_.end()) {
		map_.insert(pair<uint64_t, uint8_t>(hash_key, 1Ui8));
	}
	else {
		it->second++;
	}
}

void HistoryMap::decrease(const uint64_t hash_key) {
	auto it = map_.find(hash_key);
	if (it != map_.end() && ((--it->second) == 0Ui8)) {
		map_.erase(hash_key);
	}
	else {
		throw new exception("key not found");
	}
}

void HistoryMap::clear() {
	map_.clear();
}

bool HistoryMap::isAtMax(const uint64_t hash_key) {
	auto it = map_.find(hash_key);
	if (it == map_.end())
		return false;

	return (it->second >= MAX_COUNT);
}
