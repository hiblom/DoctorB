#pragma once
#include <string>
#include <vector>
#include <boost/optional.hpp>
#include "Position.h"

class Uci
{
public:
	Uci();
	~Uci();
	bool Execute(std::string command);
private:
	boost::optional<Position> position;
	void executeUci();
	void executeIsReady();
	void executePosition(const std::vector<std::string>& command_parts);
	void executeGo(const std::vector<std::string>& command_parts);
	void goDepth(const std::vector<std::string>& tokens);
	void goTime(const std::vector<std::string>& tokens);
	void goPerft(const std::vector<std::string>& tokens);
	void executeD();
};
