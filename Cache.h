#pragma once

#include "mset.h"

enum class Ass {
	custom, full
};

class Cache
{
public:
//	Cache() {}

	//c/b/a = nubmer of sets; cmd / b = cmd >> log2(b) + 1 which is the number of bits for blocks; mask for example: c/b/a = 8 mask = 7 (0111), c/b/a = 4 mask = 3 (0011), c/b/a = 1 mask = 0 (0000)
	Cache(CConfig info, Ass _ass) : sets(), mask(info.c/info.b/info.a - 1), pos(info.b){
		if(_ass == Ass::full){
			info.a = info.c / info.b;
		}

		mask = info.c / info.b / info.a - 1;
		size_t num_of_sets = info.c / info.b / info.a;
		assert((info.policy == "LRU" || info.policy == "RND"));
		if (info.policy == "LRU") {
			if (info.isAlloc == "alloc") {
				for (size_t i = 0; i < num_of_sets; i++) {
					sets.push_back(new SetLRU<true>(info.a, info.c));
				}
			}
			else {
				for (size_t i = 0; i < num_of_sets; i++) {
					sets.push_back(new SetLRU<false>(info.a, info.c));
				}
			}
		}
		else if (info.policy == "RND") {
			if (info.isAlloc == "alloc") {
				for (size_t i = 0; i < num_of_sets; i++) {
					sets.push_back(new SetRND<true>(info.a, info.c));
				}
			}
			else {
				for (size_t i = 0; i < num_of_sets; i++) {
					sets.push_back(new SetRND<false>(info.a, info.c));
				}
			}
		}
	}

	Cache(Cache &rhs) = delete;
	Cache(Cache &&rhs) = delete;
	Cache& operator=(Cache &rhs) = delete;
	Cache& operator=(Cache &&rhs) = delete;

	~Cache() {
		for (auto eachSet : sets) {
			delete eachSet;
		}
	}

	Res find(size_t cmd, size_t mode) {
		size_t index = cmd / pos;
		index = mask & index;
		
		return sets[index]->find(cmd, mode);
	}

	std::vector<size_t> flush() {
		std::vector<size_t> dirtyList;
		for (auto oneSet : sets) {
			std::vector<size_t> dirtyListOfOneSet = oneSet->flush();
			dirtyList.insert(dirtyList.begin(), dirtyListOfOneSet.begin(), dirtyListOfOneSet.end());
		}

		return dirtyList;
	}

	void disp() {
		int i = 0;
		for (auto set : sets) {
			std::cout << i << ": " << std::endl;
			set->disp();
			i++;
		}
	}
private:

	std::vector<Set*> sets;
	size_t mask;
	size_t pos;
};