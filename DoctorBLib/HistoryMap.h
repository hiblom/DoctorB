#pragma once
#include <cstdint>
#include <map>
class HistoryMap
{
public:
	HistoryMap();
	~HistoryMap();
	void Increase(const uint64_t hash_key);
	void Descrease(const uint64_t hash_key);
	void Clear();
	bool IsAtMax(const uint64_t hash_key);
private:
	static const uint8_t MAX_COUNT = 2;
	std::map<uint64_t, uint8_t> map_;
};

