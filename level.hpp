#pragma once

/*=============================================
Level的函数说明
Res find(size_t cmd, size_t mode)
	接收地址cmd和操作mode。更新缓存的内容并返回Res。
	Res.fisrt = 1: HIT, Res.first = 0: FALSE;
	Res.second.first = kickedTag
	Res.second.second.second = dirtyBit

double getAT()
	返回单词访问时间

==============================	
size_t getMiss(size_t i)
	返回对应miss

size_t getTotal(size_t i)
	返回对应总访问次数

size_t getCompulsoryMiss(size_t i)
	返回对应compulsory miss
	
	在分割线内
	i = 0, 读
	i = 1， 写
	i = 2， 指令
=============================================*/

//==================================================================================
/*

我还没有想好要不要用这样的结构初始化，感觉这样做有点复杂。但是我应该不会改了。

在Cache.h中定义了
class CConfig {
public:
CConfig(int _a, int _b, int _c, Victim _p, Alloc _alloc):a(_a), b(_b),c(_c),policy(_p),isAlloc(_alloc) {}
int a, b, c;
Victim policy;
Alloc isAlloc;
};

level 接收CConfig的vector，指定这级每个Cache的属性。
*/
//==================================================================================

#include <iostream>
#include "Cache.h"
#include "my_types.h"
#include <cassert>
#include <set>

class Level {
public:
	Level(double ac, std::string _alloc) :accessTime(ac), misses(3, 0), total(3, 0), visitedTags(), compulsory_misses(3, 0), conflict_misses(3, 0), alloc(_alloc) {}
	virtual Res find(size_t cmd, size_t mode) = 0;
	virtual void disp() = 0;
	size_t getMiss(size_t i) {
		assert(i < misses.size());
		return misses[i]; 
	}
	
	size_t getTotal(size_t i) { 
		assert(i < total.size());
		return total[i]; 
	}
	
	double getAT() { return accessTime; }

	size_t getCompulsoryMiss(size_t i) { return compulsory_misses[i]; }
	
	template <size_t i>
	double getMissRate() {
		if (i < 3) {
			if (total[i] == 0) {
				return 0;
			}
			else {
				return misses[i] / static_cast<double>(total[i]);
			}
		}
		else if (i == 4){
			double totalAll = 0;
			for (auto n : total) {
				totalAll += n;
			}
			if (totalAll == 0) {
				return 0;
			}
			else {
				double missAll = 0;
				for (auto n : misses) {
					missAll += n;
				}
				return static_cast<double>(missAll / totalAll);
			}
		}
		else {
			double totalData = static_cast<double>(total[0] + total[1]);
			if (totalData == 0) {
				return 0;
			}
			else {
				double missData = static_cast<double>(misses[0] + misses[1]);
				return missData / totalData * 1.0;
			}
		}
	}

	std::string getAlloc() { return alloc; }

	size_t getConfictMiss(size_t i) { return conflict_misses[i]; }

	virtual std::vector<size_t> flush() = 0;

	virtual ~Level(){}
protected:
	double accessTime;
	std::vector<size_t> misses;
	std::vector<size_t> total;
	std::set<size_t> visitedTags;
	std::vector<size_t> compulsory_misses;
	std::vector<size_t> conflict_misses;
	std::string alloc;
};

class UnifiedCache : public Level {
public:
	UnifiedCache(std::vector<CConfig> info, double ac) : Level(ac, info[0].isAlloc), cache(info[0], Ass::custom), mask(~(info[0].b) + 1), cache_full(info[0], Ass::full) {}
	UnifiedCache(UnifiedCache&) = delete;
	UnifiedCache(UnifiedCache&&) = delete;
	UnifiedCache& operator=(UnifiedCache&) = delete;
	UnifiedCache& operator=(UnifiedCache&&) = delete;

	virtual Res find(size_t cmd, size_t mode) override {
		total[mode]++;
		Res res = cache.find(cmd, mode);
		
		Res res_full = cache_full.find(cmd, mode);

		if (!res.first) {
			misses[mode]++;
			
			//check if it is a compulsory miss. if so, miss++
			if (visitedTags.find(cmd & mask) == visitedTags.end()) {
				visitedTags.insert(cmd & mask);
				compulsory_misses[mode]++;
			}

			//check if it is a conflict miss
			else if(res_full.first){
				conflict_misses[mode]++;
			}
		}

		return res;
	}

	virtual std::vector<size_t> flush() override {
		return cache.flush();
	}
	
	virtual void disp() override {
		cache.disp();
		std::cout << std::endl;
	}
private:
	Cache cache;
	size_t mask;
	Cache cache_full;
};

class SplitCache : public Level {
public:
	SplitCache(std::vector<CConfig> info, double ac) : Level(ac, info[0].isAlloc), data(info[0], Ass::custom), inst(info[1], Ass::custom), masks({ ~(info[0].b) + 1, ~(info[1].b) + 1}), data_full(info[0], Ass::full), inst_full(info[1], Ass::full) {}
	SplitCache(SplitCache&) = delete;
	SplitCache(SplitCache&&) = delete;
	SplitCache& operator=(SplitCache&) = delete;
	SplitCache& operator=(SplitCache&&) = delete;
	virtual Res find(size_t cmd, size_t mode) override {
		total[mode]++;
		Res res;
		Res res_full;

		if (mode == 2) {
			res = inst.find(cmd, mode);
			res_full = inst_full.find(cmd, mode);
		}
		else {
			res = data.find(cmd, mode);
			res_full = data_full.find(cmd, mode);
		}

		if (!res.first) {
			misses[mode]++;

			//check if it is a compulsory miss, if so, miss++
			if (visitedTags.find(cmd & masks[mode / 2]) == visitedTags.end()) {
				visitedTags.insert(cmd & masks[mode / 2]);
				compulsory_misses[mode]++;
			}
			//check if it is a conflict miss
			else if(res_full.first){
				conflict_misses[mode]++;
			}
		}
		return res;
	}

	virtual std::vector<size_t> flush() override {
		return data.flush();
	}

	virtual void disp() override {
		std::cout << "======data======" << std::endl;
		data.disp();
		std::cout << "======inst======" << std::endl;
		inst.disp();
		std::cout << std::endl;
	}
private:
	Cache data, inst;
	std::vector<size_t> masks;
	Cache data_full, inst_full;
};