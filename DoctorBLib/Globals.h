#pragma once
#include <atomic>

struct Globals {
	static std::atomic<bool> out_of_book;
	static std::atomic<bool> searching;
	static std::atomic<bool> stop;
};