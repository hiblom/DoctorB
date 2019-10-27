#include "stdafx.h"
#include "Uci.h"
#include <string>
#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <chrono>
#include "App.h"
#include "Position.h"
#include "Parser.h"
#include "Perft.h"

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
	else if (command_parts[0] == "go") {
		executeGo(command_parts);
		return true;
	}
	else if (command_parts[0] == "stop")
		return true;
	else if (command_parts[0] == "ponderhit")
		return true;
	else if (command_parts[0] == "quit")
		return false;
	else if (command_parts[0] == "d") {
		executeD();
		return true;
	}
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
		cout << "Invalid position command" << endl;
		return;
	}

	position = Position();

	vector<string> position_tokens(command_parts.begin() + 1, command_parts.end());
	Parser::ParsePosition(position_tokens, position.value());
}

void Uci::executeGo(const std::vector<std::string>& command_parts) {
	if (command_parts.size() < 2) {
		cout << "Invalid go command" << endl;
		return;
	}

	vector<string> tokens(command_parts.begin() + 1, command_parts.end());
	if (command_parts[1] == "perft") {
		goPerft(tokens);
	}
}

void Uci::goPerft(const vector<string>& tokens) {
	if (tokens.size() != 2) {
		cout << "Number of perft parameters must be 1" << endl;
		return;
	}

	int depth = stoi(tokens[1]);
	if (depth < 1 || depth > 20) {
		cout << "Perft depth must be between 1 and 20" << endl;
		return;
	}

	if (!position.has_value()) {
		cout << "Position must be set for perft" << endl;
		return;
	}
	
	auto start_time = chrono::system_clock::now();

	Perft perft(position.value());
	perft.SetDepth(depth);
	uint64_t count = perft.Go();

	auto end_time = chrono::system_clock::now();

	chrono::duration<double> elapsed_seconds = end_time - start_time;

	cout 
		<< "Perft result: " << count
		<< " nodes, elapsed time: " << (int)(elapsed_seconds.count() * 1000)
		<< " ms, nodes per second: " << (int)(count / elapsed_seconds.count()) << endl;
}

void Uci::executeD() {
	if (!position.has_value()) {
		cout << "No position set";
	}
	cout << position.value().ToString();
}

