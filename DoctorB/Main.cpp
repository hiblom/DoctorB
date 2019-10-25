#include <iostream>
#include <string>
#include <cstdio>
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
	MoveBoard::GetInstance().Initialize();
	Uci uci;
	
	//main loop to receive commands
	string line;
	while (getline(cin, line)) {
		if (!uci.Execute(line))
			break;
	}

	return 0;
}