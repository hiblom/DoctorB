#include <iostream>
#include <string>
#include <cstdio>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include "Uci.h"
#include "MoveBoard.h"
#include <Console.h>

using namespace std;

int main() {
	//set UTF-8 as default
	SetConsoleOutputCP(CP_UTF8);
	setvbuf(stdout, nullptr, _IOFBF, 1000);

	Console::getInstance() << "Welcome to DoctorB chess engine" << endl;
	Console::getInstance() << "Input UCI command" << endl;

	//initialize engine
	srand(static_cast<unsigned int>(time(NULL)));
	Uci uci;
	
	//main loop to receive commands
	string line;
	while (Console::getInstance().readLine(line)) {
		if (!uci.execute(line))
			break;
	}

	return 0;
}