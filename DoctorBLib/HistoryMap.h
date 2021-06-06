#pragma once
#include <cstdint>
#include <map>
class HistoryMap
{
public:
	HistoryMap();
	~HistoryMap();
	void increase(const uint64_t hash_key);
	void decrease(const uint64_t hash_key);
	void clear();
	bool isAtMax(const uint64_t hash_key);
private:
	static const uint8_t MAX_COUNT = 2;
	std::map<uint64_t, uint8_t> map_;
};

