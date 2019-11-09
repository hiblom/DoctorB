#pragma once
#include <unordered_map>
#include "Move.h"

class TranspositionTable
{
public:
	struct IdentityHasher {
		std::size_t operator()(const uint64_t k) const {
			return (std::size_t)k;
		}
	};

	struct Entry {
		Move best_move;
		
		Entry() {}
		Entry(Move move) : best_move(move) {}
	};

	~TranspositionTable();
	static TranspositionTable& GetInstance();
	TranspositionTable(const TranspositionTable&) = delete;
	void operator=(const TranspositionTable&) = delete;
	bool AddEntry(const uint64_t key, const Entry entry);
	bool FindEntry(const uint64_t key, Entry& entry);
	void Clear();
	void Reset();
private:
	TranspositionTable();
	std::unordered_map<uint64_t, Entry, IdentityHasher> map; //TODO maybe put TT on heap?
	size_t max_size;
};
