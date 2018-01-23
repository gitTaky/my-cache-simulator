#include "printResult.h"

void printResult(Memories &custom, double ac) {
	if ((*custom[0]).getTotal(0) + (*custom[0]).getTotal(1) + (*custom[0]).getTotal(2) == 0) {
		std::cerr << "total = 0" << std::endl;
	}

	std::cout.width(10);
	std::cout.precision(4);
	std::cout.setf(std::ios::fixed, std::ios::floatfield);

	for (size_t i = 0; i < custom.size(); i++) {
		std::cout << "=====================================================================================================================================================================================" << std::endl;
		std::cout << "Level" << i + 1 << ": " << std::endl << std::endl;

		std::cout << "Metrics\t\t\t\t\t" << "Total\t\t\t\t" << "Instrn\t\t\t\t" << "Data                             \t" << "Read\t\t\t\t" << "Write" << std::endl;
		std::cout << "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << std::endl;

		size_t total = (*custom[i]).getTotal(0) + (*custom[i]).getTotal(1) + (*custom[i]).getTotal(2);
		size_t dataTotal = (*custom[i]).getTotal(0) + (*custom[i]).getTotal(1);
		std::cout << "Demand Fetches\t\t\t\t" << total << "\t\t\t\t" << (*custom[i]).getTotal(2) << "\t\t\t\t" << dataTotal << "                        \t     \t" << (*custom[i]).getTotal(0) << "\t\t\t\t" << (*custom[i]).getTotal(1) << std::endl;
		
		if (total == 0) {
			std::cout << "Fraction of total\t\t\t" << 0.0 << "\t\t\t\t" << 0.0 << "\t\t\t\t" << 0.0 << "                             \t" << 0.0 << "\t\t\t\t" << 0.0 << std::endl;
		}
		else {
			std::cout << "Fraction of total\t\t\t" << 1.0 << "\t\t\t\t" << (*custom[i]).getTotal(2) / static_cast<double>(total) << "\t\t\t\t" << dataTotal / static_cast<double>(total) << "                             \t" << (*custom[i]).getTotal(0) / static_cast<double>(total) << "\t\t\t\t" << (*custom[i]).getTotal(1) / static_cast<double>(total) << std::endl;
		}

		std::cout << std::endl;


		size_t totalMiss = (*custom[i]).getMiss(0) + (*custom[i]).getMiss(1) + (*custom[i]).getMiss(2);
		size_t dataMiss = (*custom[i]).getMiss(0) + (*custom[i]).getMiss(1);
		std::cout << "Demand Misses" << "\t\t\t\t" << totalMiss << "\t\t\t\t" << (*custom[i]).getMiss(2) << "\t\t\t\t" << dataMiss << "                        \t     \t" << (*custom[i]).getMiss(0) << "\t\t\t\t" << (*custom[i]).getMiss(1) << std::endl;
		std::cout << "Demand miss rate" << "\t\t\t" << custom[i]->getMissRate<4>() << "\t\t\t\t" << custom[i]->getMissRate<2>() << "\t\t\t\t" << custom[i]->getMissRate<5>() << "                        \t     \t" << custom[i]->getMissRate<0>() << "\t\t\t\t" << custom[i]->getMissRate<1>() << std::endl;
		std::cout << std::endl;

		std::cout << "Compulsory misses" << "\t\t\t" << custom[i]->getCompulsoryMiss(0) + custom[i]->getCompulsoryMiss(1) + custom[i]->getCompulsoryMiss(2) << "\t\t\t\t" << custom[i]->getCompulsoryMiss(2) << "\t\t\t\t" << custom[i]->getCompulsoryMiss(0) + custom[i]->getCompulsoryMiss(1) << "                        \t     \t" << custom[i]->getCompulsoryMiss(0) << "\t\t\t\t" << custom[i]->getCompulsoryMiss(1) << std::endl;
	
		size_t conflictMiss[3];
		for (size_t j = 0; j < 3; j++) {
			conflictMiss[j] = custom[i]->getConfictMiss(j);
		}

		size_t capatityMiss[3];
		for (size_t j = 0; j < 3; j++) {
			capatityMiss[j] = custom[i]->getMiss(j) - custom[i]->getCompulsoryMiss(j) - conflictMiss[j];
		}

		std::cout << "Conflict misses" << "\t\t\t\t" << conflictMiss[0] + conflictMiss[1] + conflictMiss[2] << "\t\t\t\t" << conflictMiss[2] << "                    \t     \t" << conflictMiss[0] + conflictMiss[1] << "                          \t\t" << conflictMiss[0] << "\t\t\t\t" << conflictMiss[1] << std::endl;
		std::cout << "Capacity misses" << "\t\t\t\t" << capatityMiss[0] + capatityMiss[1] + capatityMiss[2] << "\t\t\t\t" << capatityMiss[2] << "                    \t     \t" << capatityMiss[0] + capatityMiss[1] << "                          \t\t" << capatityMiss[0] << "\t\t\t\t" << capatityMiss[1] << std::endl;
		
		std::cout << std::endl;
		std::cout << std::endl;
	}

	std::cout << "average access time: " << ac << std::endl;
}