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
#include "Evaluator.h"
#include "Polyglot.h"
#include "MoveGenerator.h"
#include "Options.h"
#include "TranspositionTable.h"
#include "Globals.h"
#include "Constants.h"
#include "Console.h"

using namespace std;
using namespace boost::algorithm;

Uci::Uci() {
	thread_pool_.resize(1);
}

Uci::~Uci() {
}

bool Uci::execute(string command) {
	vector<string> command_parts;
	split(command_parts, command, is_any_of(" "));

	if (Globals::searching) {
		//only allowed uci commands are stop and quit
		if (iequals(command_parts[0], "stop")) {
			Globals::stop = true;
			searching_.wait();
			Globals::stop = false;
			return true;
		}
		else if (iequals(command_parts[0], "quit")) {
			Globals::stop = true;
			searching_.wait();
			Globals::stop = false;
			return false;
		}
		else {
			Console::getInstance() << "Invalid command while searching" << endl;
			return true;
		}
	}

	if (iequals(command_parts[0], "uci")) {
		executeUci();
		return true;
	}
	else if (iequals(command_parts[0], "debug")) {
		return true;
	}
	else if (iequals(command_parts[0],"isready")) {
		executeIsReady();
		return true;
	}
	else if (iequals(command_parts[0],"setoption")) {
		executeSetOption(command_parts);
		return true;
	}
	else if (iequals(command_parts[0], "register")) {
		return true;
	}
	else if (iequals(command_parts[0],"ucinewgame")) {
		executeUciNewGame();
		return true;
	}
	else if (iequals(command_parts[0], "position")) {
		executePosition(command_parts);
		return true;
	}
	else if (iequals(command_parts[0], "go")) {
		executeGo(command_parts);
		return true;
	}
	else if (iequals(command_parts[0], "stop")) {
		return true;
	}
	else if (iequals(command_parts[0], "ponderhit")) {
		return true;
	}
	else if (iequals(command_parts[0], "quit")) {
		return false;
	}
	else if (iequals(command_parts[0], "d")) {
		executeD();
		return true;
	}
	else {
		Console::getInstance() << "Invalid UCI command" << endl;
		return true;
	}
}

void Uci::executeUci() {
	Console::getInstance() << "id name " << App::Name << " " << App::Version << endl;
	Console::getInstance() << "id author " << App::Author << endl;
	Console::getInstance() << "option name Hash type spin default " << Constants::DEFAULT_HASH << " min 1 max 256" << endl;
	Console::getInstance() << "option name OwnBook type check default " << (Constants::DEFAULT_OWN_BOOK ? "true" : "false") << endl;
	Console::getInstance() << "option name OwnBookPath type string default " << Constants::DEFAULT_OWN_BOOK_PATH << endl;
	Console::getInstance() << "uciok" << endl;
}

void Uci::executeUciNewGame() {
	position_.reset();
	history_.clear();
	Globals::out_of_book = false;
}

void Uci::executeIsReady() {
	Console::getInstance() << "readyok" << endl;
}

void Uci::executePosition(const std::vector<std::string>& command_parts) {
	if (command_parts.size() < 2) {
		Console::getInstance() << "Invalid position command" << endl;
		return;
	}

	position_ = Position();

	vector<string> position_tokens(command_parts.begin() + 1, command_parts.end());
	Parser::parsePosition(position_tokens, position_.value(), history_);
}

void Uci::executeGo(const vector<string>& command_parts) {
	if (command_parts.size() < 2) {
		Console::getInstance() << "Invalid go command" << endl;
		return;
	}

	if (!position_.has_value()) {
		Console::getInstance() << "No position set" << endl;
		return;
	}

	vector<string> tokens(command_parts.begin() + 1, command_parts.end());

	if (iequals(command_parts[1], "depth")) {
		goDepth(tokens);
	}
	if (iequals(command_parts[1], "infinite")) {
		goDepth(tokens);
	}
	else if (iequals(command_parts[1], "wtime") || iequals(command_parts[1], "btime") || iequals(command_parts[1], "winc") || iequals(command_parts[1], "binc")) {
		goTime(tokens);
	}
	else if (iequals(command_parts[1], "movetime")) {
		goTime(tokens);
	}
	else if (iequals(command_parts[1], "perft")) {
		goPerft(tokens);
	}
}

void Uci::executeSetOption(const std::vector<std::string>& command_parts) {
	if (command_parts.size() < 5) {
		Console::getInstance() << "Invalid setoption command" << endl;
		return;
	}

	string name = command_parts[2];
	if (iequals(name, "hash")) {
		int hashValue = stoi(command_parts[4]);
		if (hashValue < 1 || hashValue > 256) {
			Console::getInstance() << "Invalid Hash value" << endl;
			return;
		}

		Options::Hash = hashValue;
		TranspositionTable::getInstance().reset();
	}
	else if (iequals(name, "ownbook")) {
		string ownBookValue = command_parts[4];
		if (!iequals(ownBookValue, "true") && !iequals(ownBookValue, "false"))
		{
			Console::getInstance() << "Invalid OwnBook value" << endl;
			return;
		}
		Options::OwnBook = (iequals(ownBookValue, "true"));
	}
	else if (iequals(name, "ownbookpath")) {
		//filename could contain spaces, so there could be more than 5 command parts
		vector<string> filename_parts(command_parts.begin() + 4, command_parts.end());
		string filename = join(filename_parts, " ");

		Options::OwnBookPath = filename;
		if (!Polyglot::getInstance().open()) {
			Console::getInstance() << "Error opening file at provided OwnBookPath value" << endl;
			return;
		}
	}
	else {
		Console::getInstance() << "Invalid option" << endl;
	}
}

void Uci::goDepth(const vector<string>& tokens) {
	int depth = 0;
	if (iequals(tokens[0], "infinite")) {
		if (tokens.size() != 1) {
			Console::getInstance() << "No extra infinite parameters allowed" << endl;
			return;
		}
		depth = INT_MAX; //not exactly infinite, but should be enough
	}
	else if (iequals(tokens[0], "depth")) {
		if (tokens.size() != 2) {
			Console::getInstance() << "Number of depth parameters must be 1" << endl;
			return;
		}

		depth = stoi(tokens[1]);
		if (depth < 1) {
			Console::getInstance() << "Depth must be greater than 1" << endl;
			return;
		}
	}

	Position& position = position_.value();
	HistoryMap& history = history_;

	searching_ = thread_pool_.push([position, history, depth](int id) {
		Globals::searching = true;
		Searcher search(position, history);
		search.goDepth(depth);
		Globals::searching = false;
	});
}

void Uci::goTime(const vector<string>& tokens) {
	int wtime = 0;
	int btime = 0;
	int winc = 0;
	int binc = 0;
	int movestogo = 0;

	int i = 0;
	while (i < tokens.size()) {
		if (iequals(tokens[i], "wtime"))
			wtime = stoi(tokens[++i]);
		else if (iequals(tokens[i], "btime"))
			btime = stoi(tokens[++i]);
		else if (iequals(tokens[i], "winc"))
			winc = stoi(tokens[++i]);
		else if (iequals(tokens[i], "binc"))
			binc = stoi(tokens[++i]);
		else if (iequals(tokens[i], "movestogo"))
			movestogo = stoi(tokens[++i]);
		else if (iequals(tokens[i], "movetime")) {
			wtime = stoi(tokens[++i]);
			btime = wtime;
			movestogo = 1;
		}
		i++;
	}

	Position& position = position_.value();
	HistoryMap& history = history_;

	searching_ = thread_pool_.push([position, history, wtime, btime, winc, binc, movestogo](int id) {
		Globals::searching = true;
		Searcher search(position, history);
		search.goTime(wtime, btime, winc, binc, movestogo);
		Globals::searching = false;
	});
}

void Uci::goPerft(const vector<string>& tokens) {
	if (tokens.size() != 2) {
		Console::getInstance() << "Number of perft parameters must be 1" << endl;
		return;
	}

	int depth = stoi(tokens[1]);
	if (depth < 1 || depth > 20) {
		Console::getInstance() << "Perft depth must be between 1 and 20" << endl;
		return;
	}
	
	auto start_time = chrono::system_clock::now();

	Perft perft(position_.value());
	perft.setDepth(depth);
	uint64_t count = perft.go();

	auto end_time = chrono::system_clock::now();

	chrono::duration<double> elapsed_seconds = end_time - start_time;

	Console::getInstance() 
		<< "Perft result: " << count
		<< " nodes, elapsed time: " << static_cast<int>(elapsed_seconds.count() * 1000)
		<< " ms, nodes per second: " << static_cast<int>(count / elapsed_seconds.count()) << endl;
}

void Uci::executeD() {
	if (!position_.has_value()) {
		Console::getInstance() << "No position set" << endl;
		return;
	}
	Console::getInstance() << position_.value().toString();
	
	Evaluator eval(position_.value());
	Score score;
	eval.evaluate(score);
	Console::getInstance() << "Evaluation: " << score.toString(Piece::COLOR_WHITE, 1) << endl;

}