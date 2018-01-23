#pragma once

#include "level.hpp"
#include <string>
#include <memory>
#include "my_types.h"
#include <fstream>

using Memories = std::vector<Level*>;

struct Init{
	Memories buildMem(std::string filename, double &atm) {
		assert(res.empty());

		std::ifstream in(filename);
		int n;
		in >> n;


		auto buildCache = [](std::ifstream& in) {
			size_t a, b, c;
			in >> a >> b >> c;
			std::string alloc, victim;
			in >> alloc >> victim;
			return CConfig(a, b, c, victim, alloc);
		};


		for (int i = 0; i < n; i++) {
			std::string type;
			in >> type;
			if (type == "unified") {
				std::vector<CConfig> config;
				config.push_back(buildCache(in));
				double accessTime;
				in >> accessTime;
				res.push_back(new UnifiedCache(config, accessTime));
			}
			else if (type == "split") {
				std::vector<CConfig> config;
				for (int j = 0; j < 2; j++) {
					config.push_back(buildCache(in));
				}
				double accessTime;
				in >> accessTime;
				res.push_back(new SplitCache(config, accessTime));
			}
		}

		in >> atm;

		return res;
	}

	~Init() {
		for (auto i : res) {
			delete i;
		}
	}

	Memories res;
};