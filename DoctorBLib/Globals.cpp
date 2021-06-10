#include "stdafx.h"

#include "Globals.h"

using namespace std;

atomic<bool> Globals::out_of_book = false;
atomic<bool> Globals::searching = false;
atomic<bool> Globals::stop = false;
