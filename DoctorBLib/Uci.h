#pragma once
#include <string>
#include <vector>
#include <boost/optional.hpp>
#include "Position.h"
#include "HistoryMap.h"

class Uci
{
public:
	Uci();
	~Uci();
	bool Execute(std::string command);
private:
	boost::optional<Position> position_;
	HistoryMap history_;
	void executeUci();
	void executeUciNewGame();
	void executeIsReady();
	void executePosition(const std::vector<std::string>& command_parts);
	void executeGo(const std::vector<std::string>& command_parts);
	void executeSetOption(const std::vector<std::string>& command_parts);
	void goDepth(const std::vector<std::string>& tokens);
	void goTime(const std::vector<std::string>& tokens);
	void goPerft(const std::vector<std::string>& tokens);
	void executeD();

	//temp
	void TestPolyglot();
};
