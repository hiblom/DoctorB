﻿#include <iostream>
#include <string>
#include <cstdio>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include "Uci.h"
#include "MoveBoard.h"

using namespace std;

int main() {
	//set UTF-8 as default
	SetConsoleOutputCP(CP_UTF8);
	setvbuf(stdout, nullptr, _IOFBF, 1000);

	cout << "Welcome to DoctorB chess engine" << endl;
	cout << "Input UCI command" << endl;

	//initialize engine
	srand(static_cast<unsigned int>(time(NULL)));
	Uci uci;
	
	//main loop to receive commands
	string line;
	while (getline(cin, line)) {
		if (!uci.execute(line))
			break;
	}

	return 0;
}