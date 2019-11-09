#pragma once
#include <cstdint>

class Options
{
public:
	uint32_t MaxHashSizeMb;
	~Options();
	static Options& GetInstance();
	Options(const Options&) = delete;
	void operator=(const Options&) = delete;
private:
	Options();
	
};

