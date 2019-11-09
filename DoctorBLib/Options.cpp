#include "stdafx.h"
#include "Options.h"

Options::Options() {
	//set defaults
	MaxHashSizeMb = 16;
}


Options::~Options() {
}

Options& Options::GetInstance() {
	static Options instance;
	return instance;
}
