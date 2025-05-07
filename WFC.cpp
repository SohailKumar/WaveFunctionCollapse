#include "WFC.h"
#include <queue>
#include <set>
#include <stacktrace>

WFC::WFC(const int gridWidth, const int gridHeight)
{
	this->adjacencyRules[{ Tiles::BLANK, Direction::NORTH }] = { Tiles::UP, Tiles::BLANK };
	this->adjacencyRules[{ Tiles::BLANK, Direction::EAST }] = { Tiles::RIGHT, Tiles::BLANK };
	this->adjacencyRules[{ Tiles::BLANK, Direction::WEST }] = { Tiles::LEFT, Tiles::BLANK };
	this->adjacencyRules[{ Tiles::BLANK, Direction::SOUTH }] = { Tiles::DOWN, Tiles::BLANK };

	this->adjacencyRules[{ Tiles::DOWN, Direction::NORTH }] = { Tiles::BLANK, Tiles::UP};
	this->adjacencyRules[{ Tiles::DOWN, Direction::EAST }] = { Tiles::DOWN, Tiles::LEFT, Tiles::UP };
	this->adjacencyRules[{ Tiles::DOWN, Direction::WEST }] = { Tiles::DOWN, Tiles::RIGHT, Tiles::UP };
	this->adjacencyRules[{ Tiles::DOWN, Direction::SOUTH }] = { Tiles::LEFT, Tiles::RIGHT, Tiles::UP };

	this->adjacencyRules[{ Tiles::LEFT, Direction::NORTH }] = { Tiles::DOWN, Tiles::LEFT, Tiles::RIGHT };
	this->adjacencyRules[{ Tiles::LEFT, Direction::EAST }] = { Tiles::BLANK, Tiles::RIGHT };
	this->adjacencyRules[{ Tiles::LEFT, Direction::WEST }] = { Tiles::DOWN, Tiles::RIGHT, Tiles::UP };
	this->adjacencyRules[{ Tiles::LEFT, Direction::SOUTH }] = { Tiles::LEFT, Tiles::RIGHT, Tiles::UP };

	this->adjacencyRules[{ Tiles::RIGHT, Direction::NORTH }] = { Tiles::DOWN, Tiles::LEFT, Tiles::RIGHT };
	this->adjacencyRules[{ Tiles::RIGHT, Direction::EAST }] = { Tiles::DOWN, Tiles::LEFT, Tiles::UP };
	this->adjacencyRules[{ Tiles::RIGHT, Direction::WEST }] = { Tiles::BLANK, Tiles::LEFT };
	this->adjacencyRules[{ Tiles::RIGHT, Direction::SOUTH }] = { Tiles::LEFT, Tiles::RIGHT, Tiles::UP };

	this->adjacencyRules[{ Tiles::UP, Direction::NORTH }] = { Tiles::DOWN, Tiles::LEFT, Tiles::RIGHT };
	this->adjacencyRules[{ Tiles::UP, Direction::EAST }] = { Tiles::DOWN, Tiles::LEFT, Tiles::UP };
	this->adjacencyRules[{ Tiles::UP, Direction::WEST }] = { Tiles::DOWN, Tiles::RIGHT, Tiles::UP };
	this->adjacencyRules[{ Tiles::UP, Direction::SOUTH }] = { Tiles::BLANK, Tiles::DOWN };

	this->gridWidth = gridWidth;
	this->gridHeight = gridHeight;
	this->grid = std::vector<std::vector<WFCCell>>(gridWidth, std::vector<WFCCell>(gridHeight));

	Reset();

	PrintEntropies();
}

void WFC::PrintEntropies() {
	// Print the entropy of each cell in the grid 
	std::cout << "Entropies:" << std::endl;
	for (int y = 0; y < this->gridHeight; ++y) {
		for (int x = 0; x < this->gridWidth; ++x) {
			std::cout << grid[y][x].entropy << " ";
		}
		std::cout << std::endl;
	}
}

void WFC::Collapse(std::vector<std::vector<Tiles>> &outputWFC) {
	//outputWFC = std::vector<std::vector<Tiles>>(this->gridWidth, std::vector<Tiles>(this->gridHeight, Tiles::BLANK));
	//Reset();
	//outputWFC[0][0] = CollapseCell(0, 0);
	//outputWFC[0][0] = this->grid[0][0].possibleTiles[0];

	// Find all cells with entropy > 1
	// Choose one randomly from these cells
	int x, y;
	bool done = false;
	bool redo = false;
	bool oneAtATime = true;
	FindLowestEntropyCell(x, y, done);
	if (done) {
		std::cout << "Resetting for new WFC" << std::endl;
		// Reset the grid and outputWFC
		this->Reset();
		outputWFC = std::vector<std::vector<Tiles>>(this->gridWidth, std::vector<Tiles>(this->gridHeight, Tiles::BLANK));
	}

	while (done == false) {
		FindLowestEntropyCell(x, y, done);
		if (done) {
			std::cout << "All cells collapsed" << std::endl;
			return;
		}

		// Collapse that cell and set the outputWFC[x][y] to the tile
		outputWFC[y][x] = CollapseCell(x, y);

		// Update the entropies of the neighboring cells
		UpdateEntropies(x, y, redo);
		if (redo) {
			std::cout << "Redoing..." << std::endl;
			// Reset the grid and outputWFC
			this->Reset();
			outputWFC = std::vector<std::vector<Tiles>>(this->gridWidth, std::vector<Tiles>(this->gridHeight, Tiles::BLANK));
		}
		if (oneAtATime = true) {
			break;
		}
	// Repeat until all cells collapsed or a cell has 0 valid tiles
	}

	//std::cout << std::endl;
	//std::cout << "Collapsed" << std::endl;
	//for (int y = 0; y < this->gridHeight; ++y) {
	//	for (int x = 0; x < this->gridWidth; ++x) {
	//		std::cout << static_cast<int>(outputWFC[y][x]) << " ";
	//	}
	//	std::cout << std::endl;
	//}
	//PrintEntropies();

}

// Privates

void WFC::Reset() {
	for (int y = 0; y < this->gridHeight; ++y) {
		for (int x = 0; x < this->gridWidth; ++x) {
			this->grid[y][x] = WFCCell{ {Tiles::BLANK, Tiles::DOWN, Tiles::LEFT, Tiles::RIGHT, Tiles::UP}, false, 5.0f };
		}
	}
}

void WFC::FindLowestEntropyCell(int& x, int& y, bool& done) {
	float minEntropy = std::numeric_limits<float>::max();
	std::vector<std::pair<int, int>> candidates;

	for (int y = 0; y < this->gridHeight; ++y) {
		for (int x = 0; x < this->gridWidth; ++x) {
			const WFCCell& cell = this->grid[y][x];
			if (cell.collapsed) continue;

			if (cell.entropy < minEntropy) {
				minEntropy = cell.entropy;
				candidates.clear();
				candidates.push_back({ x, y });
			}
			else if (cell.entropy == minEntropy) {
				candidates.push_back({ x, y });
			}
		}
	}

	if (candidates.empty()) {
		done = true;
		return;
	}
	int randomIndex = rand() % candidates.size();
	x = candidates[randomIndex].first;
	y = candidates[randomIndex].second;
	done = false;
	return;
}

Tiles WFC::CollapseCell(int x, int y) {
	// Collapse the cell at (x, y) to a random tile
	this->grid[y][x].collapsed = true;
	this->grid[y][x].entropy = 1.0f; // Set entropy to 1 after collapsing

	// Randomly select a tile from the possible tiles
	int randomIndex = rand() % (this->grid[y][x].possibleTiles.size());
	//std::cout << "size: " << this->grid[x][y].possibleTiles.size() << std::endl;
	//std::cout << "randomly collapsing cell at (" << x << ", " << y << ")" << ": " << randomIndex << ", " << static_cast<int>(this->grid[x][y].possibleTiles[randomIndex]) << std::endl;
	this->grid[y][x].possibleTiles = { this->grid[y][x].possibleTiles[randomIndex] };
	return this->grid[y][x].possibleTiles[0];
}

void WFC::UpdateEntropies(int startX, int startY, bool& redo) {
	// Update the entropies of the neighboring cells
	std::queue<std::pair<int, int>> toVisit;
	toVisit.push({startX, startY}); // starting point

	std::vector<std::pair<std::string, std::pair<int, int>>> directions = {
		{"NORTH",	{ 0, -1}},
		{"EAST",	{ 1,  0}},
		{"SOUTH",	{ 0,  1}},
		{"WEST",	{-1,  0}}
	};

	while (!toVisit.empty()) {
		auto [x, y] = toVisit.front();
		toVisit.pop();
		
		WFCCell& currentCell = this->grid[y][x];

		// for each neighbor
		for (auto [dir, offset] : directions) {
			int newX = x + offset.first;
			int newY = y + offset.second;

			// check for edges
			if (newX < 0 || newX >= this->gridWidth || newY < 0 || newY >= this->gridHeight) continue;

			WFCCell& neighbor = this->grid[newY][newX];

			if (neighbor.collapsed) continue;
			

			bool changed = false;
			//Update neighbor possibleTiles

			std::set<Tiles> allowedNeighborTiles = {};
			for (auto tile : currentCell.possibleTiles) {
				if (dir == "NORTH") {
					std::vector<Tiles> toAdd = adjacencyRules[{tile, Direction::NORTH}];
					for (auto t : toAdd) {
						allowedNeighborTiles.insert(t);
					}
				}
				else if (dir == "EAST") {
					std::vector<Tiles> toAdd = adjacencyRules[{tile, Direction::EAST}];
					for (auto t : toAdd) {
						allowedNeighborTiles.insert(t);
					}
				}
				else if (dir == "SOUTH") {
					std::vector<Tiles> toAdd = adjacencyRules[{tile, Direction::SOUTH}];
					for (auto t : toAdd) {
						allowedNeighborTiles.insert(t);
					}
				}
				else if (dir == "WEST") {
					std::vector<Tiles> toAdd = adjacencyRules[{tile, Direction::WEST}];
					for (auto t : toAdd) {
						allowedNeighborTiles.insert(t);
					}
				}
			}


			// Check if the neighbor cell's possible tiles are a subset of the allowed tiles
			std::vector<Tiles> newPossibleTiles;
			for (auto tile : neighbor.possibleTiles) {
				if (allowedNeighborTiles.count(tile)) { //if it's found, add it to the new possible tiles
					newPossibleTiles.push_back(tile);
				}
				else {
					changed = true; //if it's not found, we need to update the entropy
				}
			}

			if (changed) {
				if (newPossibleTiles.empty()) {
					redo = true;
					std::cout << "HELLO BAD" << std::endl;
					//throw std::runtime_error("No valid tiles left for neighbor cell at (" + std::to_string(newX) + ", " + std::to_string(newY) + ")");
				}

				neighbor.possibleTiles = std::move(newPossibleTiles);
				neighbor.entropy = static_cast<float>(neighbor.possibleTiles.size());
				if (neighbor.entropy == 1.0f) {
					neighbor.entropy = 1.1f; // Set to 1.1 to avoid infinite loop
				}
				toVisit.push({ newX, newY });
			}
		}

	}
}