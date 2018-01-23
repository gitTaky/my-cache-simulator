
#include <iostream>
#include "level.hpp"
#include "init.h"
#include "printResult.h"
//#include <pthread.h>
using namespace std;  

size_t search(Memories &mem, size_t cmd, size_t mode){
		size_t count=0;
		if(mem.size()==1){//only one level
			Res level_1 = mem[0]->find(cmd, mode);
			if(level_1.first){
				//hit 
			} 
			else{//level1 miss
				count++;
				
				if(mem[0]->getAlloc()== string("alloc") && level_1.second.second==1){
					count++;
				}		
			}
		}
	    if(mem.size()==2){//has two levels
	    	Res level_1 = mem[0]->find(cmd, mode);
	    	if(!level_1.first){
				if(mode==2){//level1 read instr miss
					Res level_2 = mem[1]->find(cmd, mode);
					if(!level_2.first){
					
						count++; //mem find read instr
					}
				}
				if(mode==0){//level1 read instr miss
					Res level_2 = mem[1]->find(cmd,0);
					if(!level_2.first){
						count++; //mem find read instr
					}
					if(level_1.second.second==1){
						Res level_2_dirty = mem[1]->find(level_1.second.first,1);
						if(!level_2_dirty.first){
							count++;
						}
					} 
				}
				if(mode==1){
					//Res level_2 = mem[1]->find(cmd, mode);
					if(mem[0]->getAlloc() != string("alloc") && mem[1]->getAlloc() != string("alloc")){//not level1 level2 both not allocate
						Res level_2 = mem[1]->find(cmd,1);
						if(!level_2.first){//not find in level2
							count++;//memory write find
						}
					}
					if(mem[0]->getAlloc() == string("alloc") && mem[1]->getAlloc() != string("alloc")){//level1 allocate level2 not allocate
						Res level_2 = mem[1]->find(cmd,0);
						if(level_1.second.second==1){//level1 kick is dirty
							Res level_2_dirty = mem[1]->find(level_1.second.first,1);
							if(!level_2_dirty.first){
								count++;
							}
						}
						if(!level_2.first){
							count++;//memory write find
						}
					}
					if(mem[0]->getAlloc() != string("alloc") && mem[1]->getAlloc() ==string("alloc")){//level1 not allocate but level2 allocate
						Res level_2 = mem[1]->find(cmd,1);
						if(!level_2.first){
							count++;//memory write find
							if(level_2.second.second==1)
								count++;
						}
					}
					if(mem[0]->getAlloc() == string("alloc") && mem[1]->getAlloc() == string("alloc")){//level1 level2 both allocate 
						Res level_2 = mem[1]->find(cmd,0);
						if(level_1.second.second==1){//level1 kick is dirty
							Res level_2_dirty = mem[1]->find(level_1.second.first, 1);
							if(!level_2_dirty.first){
								count++;
							}
						}
						if(!level_2.first){
							count++;//memory write find
							if(level_2.second.second==1)
								count++;
						}
					}
				} 
			}
	    }
	return count;
}

size_t request(Memories& mem, size_t level, size_t cmd, size_t mode) {
	assert(level < mem.size());
	if (level == mem.size() - 1) {
		//lowest level
		Res res = mem[level]->find(cmd, mode);
		size_t count = 0;
		if (!res.first) {
			//miss
			//requst dram
			count++;
			if (res.second.second == true) {
				//write back to dram
				count++;
			}
		}

		return count;
	}
	else {
		//not the lowest level
		Res res = mem[level]->find(cmd, mode);
		size_t count = 0;
		if (!res.first) {
			//miss
			//request next level
			if (mode == 1 && mem[level]->getAlloc() != string("alloc")) {
				//write not alloc
				//try to write next level
				count += request(mem, level + 1, cmd, 1);
			}
			else {
				//read or fetch the missing block from next level
				count += request(mem, level + 1, cmd, mode == 2 ? 2 : 0);
				
				if (res.second.second == 1) {
					//kickout a dirty block
					//write it into the next level
					count += request(mem, level + 1, res.second.first, 1);
				}
			}
		}

		return count;
	}
}

size_t flushHelper(Memories& mem, size_t level) {
	assert(level < mem.size());
	if (level == mem.size() - 1) {
		//lowest level
		std::vector<size_t> dirtyList = mem[level]->flush();

		//write dram dirtyList.size() times
		return dirtyList.size();
	}
	else {
		//no the lowest level
		std::vector<size_t> dirtyList = mem[level]->flush();
	
		size_t count = 0;
		//flush all dirty blocks into next level
		for (auto dirtyBlock : dirtyList) {
			count += request(mem, level + 1, dirtyBlock, 1);
		}

		return count;
	}
}


size_t flush(Memories& mem) {
	size_t count = 0;

	//mem[0]->disp();

	//flush from the top level to lowest level, so that all the dirty block can be writen into dram
	for (size_t level = 0; level < mem.size(); level++) {
		count += flushHelper(mem, level);
	}

	//cout << count << endl;
	return count;
}

int main() {
	double atm;

	cout << "input the config file path:" << endl;
	string config;
	cin >> config;

	cout << "input the simulate file path:" << endl;
	string sim;
	cin >> sim;

	Init init;
	Memories mem = init.buildMem(config, atm);//attribute file
	size_t count = 0;
	int count_total=0;
	//pthread thread;
	ifstream f(sim);//test address
	while (!f.eof()) {
		size_t cmd;
		size_t mode;
		f >> mode >> hex >> cmd;
		//if (cmd == 0) { break; }
	    if(mem.size()==0){
	    	break;
		}
		count += search(mem, cmd, mode);
		//count += request(mem, 0, cmd, mode);
		count_total++;
	}

	count += flush(mem);

	//mem_full[0]->disp();
	//mem[0]->disp();

	double ac;

	if(mem.size()==1){
		ac=(mem[0]->getAT()*count_total+count*atm)/count_total;
	}
	if(mem.size()==2){
		ac=(mem[0]->getAT()*count_total + mem[1]->getAT()*(mem[1]->getTotal(0)+mem[1]->getTotal(1)+mem[1]->getTotal(2)) + count*atm)/count_total;
	}
	
	printResult(mem, ac);
	system("pause");
	
	return 0;
}
