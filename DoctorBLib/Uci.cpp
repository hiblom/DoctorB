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
#include "Searcher.h"

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

	if (!position.has_value()) {
		cout << "No position set" << endl;
		return;
	}

	vector<string> tokens(command_parts.begin() + 1, command_parts.end());

	if (command_parts[1] == "depth") {
		goDepth(tokens);
	}
	else if (command_parts[1] == "wtime" || command_parts[1] == "btime" || command_parts[1] == "winc" || command_parts[1] == "binc") {
		goTime(tokens);
	}
	else if (command_parts[1] == "perft") {
		goPerft(tokens);
	}
}

void Uci::goDepth(const vector<string>& tokens) {
	if (tokens.size() != 2) {
		cout << "Number of depth parameters must be 1" << endl;
		return;
	}

	int depth = stoi(tokens[1]);
	if (depth < 1) {
		cout << "Depth must be greater than 1" << endl;
		return;
	}

	Searcher search(position.value());
	search.GoDepth(depth);
}

void Uci::goTime(const vector<string>& tokens) {
	int wtime = 0;
	int btime = 0;
	int winc = 0;
	int binc = 0;

	int i = 0;
	while (i < tokens.size()) {
		if (tokens[i] == "wtime") {
			wtime = stoi(tokens[++i]);
		}
		else if (tokens[i] == "btime") {
			btime = stoi(tokens[++i]);
		}
		else if (tokens[i] == "winc")
			winc = stoi(tokens[++i]);
		else if (tokens[i] == "binc")
			binc = stoi(tokens[++i]);
		i++;
	}

	Searcher search(position.value());
	search.GoTime(wtime, btime, winc, binc);
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

