#pragma once
#include <vector>

enum class Direction {
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3
};

enum class Tiles {
	BLANK = 0,
	DOWN = 4,
	LEFT = 2,
	RIGHT = 3,
	UP = 1
};

typedef struct {
	std::vector<Tiles> possibleTiles;
	bool collapsed;
	float entropy; //currently just the number of possible tiles
} WFCCell;