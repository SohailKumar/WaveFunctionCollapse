#pragma once
#include <map>
#include <utility>
#include <iostream>
#include "WFCUtility.h"

class WFC {
public:
	int gridWidth;
	int gridHeight;
	std::vector<std::vector<WFCCell>> grid;
	// create map from (tile, direction) -> vector of tiles
	std::map < std::pair<Tiles, Direction>, std::vector<Tiles> > adjacencyRules;

	WFC(int gridWidth, int gridHeight);
	void Collapse(std::vector<std::vector<Tiles>> &outputWFC);
	void PrintEntropies();
private:
	void Reset();
	void FindLowestEntropyCell(int &x, int &y, bool& done);
	Tiles CollapseCell(int x, int y);
	void UpdateEntropies(int x, int y, bool& redo);
};    