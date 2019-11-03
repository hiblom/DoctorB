#include "stdafx.h"
#include "Score.h"


Score::Score() {
	value_ = 0i64;
}

Score::Score(int64_t value) {
	value_ = value;
}

int64_t Score::GetValue() {
	return value_;
}

void Score::SetValue(int64_t value) {
	value_ = value;
}


Score::~Score() {
}
