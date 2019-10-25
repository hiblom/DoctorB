#pragma once
#include <string>
#include <vector>

class Uci
{
public:
	Uci();
	~Uci();
	bool Execute(std::string command);
private:
	void executeUci();
	void executeIsReady();
	void executePosition(const std::vector<std::string>& command_parts);
};
