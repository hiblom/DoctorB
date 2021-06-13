#include "stdafx.h"
#include "Console.h"

using namespace std;

Console& Console::getInstance()
{
    static Console instance;
    return instance;
}

Console::Console() {
#if LOGGING == 1
    log_file.open("log.txt", ios::app);
#endif
}

Console::~Console()
{
#if LOGGING == 1
    log_file.close();
#endif
}
