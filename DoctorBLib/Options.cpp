#include "stdafx.h"
#include "Options.h"
#include "Constants.h"

using namespace std;

size_t Options::Hash = Constants::DEFAULT_HASH;
bool Options::OwnBook = Constants::DEFAULT_OWN_BOOK;
string Options::OwnBookPath = Constants::DEFAULT_OWN_BOOK_PATH;