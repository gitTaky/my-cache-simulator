#pragma once
#include <list>

struct wayItem {
	bool dirty;
	size_t cmd;
	std::list<size_t>::iterator it;
};