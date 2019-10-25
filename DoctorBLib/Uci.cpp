#include "stdafx.h"
#include "Uci.h"
#include <string>
#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "App.h"
#include "Position.h"
#include "Parser.h"

using namespace std;
using namespace boost::algorithm;

Uci::Uci() {
}

Uci::~Uci() {
}

bool Uci::Execute(string command) {
	vector<string> command_parts;
	split(command_parts, command, boost::is_any_of(" "));

	if (command_parts[0] == "uci") {
		executeUci();
		return true;
	}
	else if (command_parts[0] == "debug")
		return true;
	else if (command_parts[0] == "isready") {
		executeIsReady();
		return true;
	}
	else if (command_parts[0] == "setoption")
		return true;
	else if (command_parts[0] == "register")
		return true;
	else if (command_parts[0] == "ucinewgame")
		return true;
	else if (command_parts[0] == "position") {
		executePosition(command_parts);
		return true;
	}
	else if (command_parts[0] == "go")
		return true;
	else if (command_parts[0] == "stop")
		return true;
	else if (command_parts[0] == "ponderhit")
		return true;
	else if (command_parts[0] == "quit")
		return false;
	else {
		cout << "Invalid UCI command" << endl;
		return true;
	}
}

void Uci::executeUci() {
	cout << "id name " << App::Name << " " << App::Version << endl;
	cout << "id author " << App::Author << endl;
	cout << "uciok" << endl;
}

void Uci::executeIsReady() {
	cout << "readyok" << endl;
}

void Uci::executePosition(const std::vector<std::string>& command_parts) {
	if (command_parts.size() < 2) {
		cout << "Invalid position" << endl;
		return;
	}

	vector<string> position_tokens(command_parts.begin() + 1, command_parts.end());
	Position position;
	if (Parser::ParsePosition(position_tokens, position)) {
		cout << position.ToString();
	}
}

