#pragma once

#include <vector>
#include <cmath>
#include <cstdint>
#include <unordered_map>
#include <list>
#include <cstdlib>
#include <ctime>
#include <iterator>
#include "my_types.h"
#include <string>
#include <cassert>

class CConfig {
public:
	CConfig(size_t _a, size_t _b, size_t _c, std::string _p, std::string _alloc) :a(_a), b(_b), c(_c), policy(_p), isAlloc(_alloc) {}
	size_t a, b, c;
	std::string policy;
	std::string isAlloc;
};

class Set {
public:

	Set(size_t a, size_t c) : ways(a), mask(~(c / a) + 1), num_of_ways(a) {};
	virtual Res find(size_t cmd, size_t mode) = 0;

	virtual void disp() {
		for (auto pair : ways) {
			std::cout << pair.first << ", dirty: " << pair.second.dirty << std::endl;
		}
	}

	virtual std::vector<size_t> flush() = 0;

	virtual ~Set(){}
protected:
	Way ways;
	size_t mask;
	size_t num_of_ways;
};

template <bool Alloc>
class SetLRU : public Set {
public:

	using LRUNodeMap = std::unordered_map<size_t, std::list<size_t>::iterator>;


	SetLRU(size_t a, size_t c) : Set(a, c), LRU() {}
	virtual Res find(size_t cmd, size_t mode) override {
		size_t tag = cmd & mask;

		//the tag exists and valid
		if (ways.find(tag) != ways.end()) {
			//auto it = LRU.begin();
			auto it = ways[tag].it;
			//while (*it != tag) { ++it; }
			LRU.erase(it);

			LRU.push_front(tag);
			ways[tag].it = LRU.begin();

			//if write, dirty bit = 1
			if (mode == 1) {
				ways[tag].dirty = true;
			}

			return { true, { 0, false } };
		}
		//the tag doesn't exist or is invalid
		else {
			Res res = { false,{ 0, false } };
			if (!(mode == 1 && !Alloc)) {
				size_t kickedTag;
				if (LRU.size() >= num_of_ways) {

					//kick the least recent used one
					//delete the kicked tag from LRU list
					//delete the kicked tag from nodeMap
					//res get the kicked tag and its dirty bit
					//kick it from map

					kickedTag = LRU.back();
					LRU.pop_back();
					res.second = { ways[kickedTag].cmd, ways[kickedTag].dirty };
					ways.erase(kickedTag);
				}


				//update LRU
				//update nodeMap
				LRU.push_front(tag);
				ways[tag].it = LRU.begin();
				ways[tag].cmd = cmd;

				//get a new block from lower level
				if (mode == 1) {
					ways[tag].dirty = true;
				}
				else {
					ways[tag].dirty = false;
				}
			}
			//write not alloc, do nothing

			return res;
		}
	}

	virtual std::vector<size_t> flush() override {
		std::vector<size_t> dirtyList;
		for (auto oneWay : ways) {
			if (oneWay.second.dirty) {
				dirtyList.push_back(oneWay.second.cmd);
			}
		}

		return dirtyList;
	}

	virtual ~SetLRU(){}
private:
	std::list<size_t> LRU;
};

template <bool Alloc>
class SetRND : public Set {
public:
	SetRND(size_t a, size_t c) : Set(a, c) {
		srand(static_cast<unsigned int>(time(0)));
	}
	virtual Res find(size_t cmd, size_t mode) override {
		size_t tag = cmd & mask;

		//the tag exists and valid
		if (ways.find(tag) != ways.end()) {
			if (mode == 1) {
				ways[tag].dirty = true;
			}

			return { true,{ 0, false } };
		}
		//the tag doesn't exists or invalid
		else {
			Res res = { false,{ 0, false } };
			if (!(mode == 1 && !Alloc)) {
				if (ways.size() >= num_of_ways) {
					size_t kickedWay = rand() % num_of_ways;
					auto it = ways.begin();
					std::advance(it, kickedWay);
					//kick one randomly
					res.second = { it->second.cmd, it->second.dirty };
					ways.erase(it);


				}

				//get a new block from the lower level
				ways[tag].cmd = cmd;
				if (mode == 1) {
					ways[tag].dirty = true;
				}
				else {
					ways[tag].dirty = false;
				}
			}

			return res;
		}
	}

	virtual std::vector<size_t> flush() override{
		std::vector<size_t> dirtyList;
		for (auto oneWay : ways) {
			dirtyList.push_back(oneWay.second.cmd);
		}

		return dirtyList;
	}

	virtual ~SetRND(){}
};
