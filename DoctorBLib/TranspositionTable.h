#pragma once
#include <memory>
#include <unordered_map>
#include <boost/optional.hpp>
#include "Move.h"
#include "Score.h"

class TranspositionTable
{
public:
	struct IdentityHasher {
		std::size_t operator()(const uint64_t k) const {
			return (std::size_t)k;
		}
	};

	struct Entry {
		boost::optional<Move> best_move;
		boost::optional<Score> score;
		uint16_t remaining_depth;
		
		Entry() : remaining_depth(0) {}
		Entry(Move move) : best_move(move), remaining_depth(0) {}
		Entry(Move move, Score score, uint16_t remaining_depth) : best_move(move), score(score), remaining_depth(remaining_depth) {}
	};

	~TranspositionTable();
	static TranspositionTable& getInstance();
	TranspositionTable(const TranspositionTable&) = delete;
	void operator=(const TranspositionTable&) = delete;
	bool setBestMove(const uint64_t key, const Move move);
	bool setScore(const uint64_t key, const Score score, const uint16_t remaining_depth);
	bool findBestMove(const uint64_t key, Move& move);
	bool findScore(const uint64_t key, Score& score, uint16_t& remaining_depth);
	int getHashFull();
	void clear();
	void reset();
private:
	TranspositionTable();
	std::unique_ptr<std::unordered_map<uint64_t, Entry, IdentityHasher>> map = std::make_unique<std::unordered_map<uint64_t, Entry, IdentityHasher>>();
	size_t max_size;
};

