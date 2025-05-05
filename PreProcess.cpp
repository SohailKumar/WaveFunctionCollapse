#include "PreProcess.h"
#include <fstream>
#include <vector>
#include <string>
#include <print>
#include <iostream>

std::vector<std::string> getGridFromFile(std::string filename) {

    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::vector<std::string> grid;

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            grid.push_back(line);
        }
    }
}

void PreProcess::preprocess()
{
	// read level from text file 
    std::string filename = "levels/lvl_1-1.txt";
	std::vector<std::string> grid = getGridFromFile(filename);

	// for each tile/char, report all adjacencies and frequencies
}
