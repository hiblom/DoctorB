#pragma once
#include <string>
#include <vector>
#include <boost/optional.hpp>
#include "ctpl.h"
#include "Position.h"
#include "HistoryMap.h"

class Uci
{
public:
	Uci();
	~Uci();
	bool execute(std::string command);
private:
	boost::optional<Position> position_;
	HistoryMap history_;
	ctpl::thread_pool thread_pool_;
	std::future<void> searching_;
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
};
