#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <string>
#include <vector>
using namespace std;

// Circuit Board Size
constexpr size_t width = 20'000;
constexpr size_t height = 20'000;

constexpr int origin = -1;

struct line {
	char dir;
	size_t steps;
};

// Read a single wire from a stream
vector<line> readNextWire(ifstream &in) {
	vector<line> wire;

	// Read line
	string line;
	getline(in, line);

	// Read moves within line
	stringstream ss(line);
	string move;
	while (getline(ss, move, ',')) {
		// extract direction first, then number of steps
		char dir = move[0];
		move[0] = ' ';
		wire.push_back({ dir, stoul(move) });
	}

	return wire;
}

int main() {
	ifstream in("input");

	vector<line> wire1(readNextWire(in));
	vector<line> wire2(readNextWire(in));

	// Allocate circuit board space and zero initialize
	int *grid = new int[width * height]();
	int startX = width / 2;
	int startY = height / 2;
	grid[startY*width + startX] = origin;

	auto x = startX;
	auto y = startY;

	size_t numSteps = 0;

	// Process first wire
	for (const auto &curLine : wire1) {
		for (auto i=0; i < curLine.steps; i++) {
			// Take a step
			switch(curLine.dir) {
				case 'R': x++;
					break;
				case 'L': x--;
					break;
				case 'U': y--;
					break;
				case 'D': y++;
					break;
			}

			numSteps++;
			
			// Only count the first time a wire crosses a point
			if (grid[y*width + x] == 0)
				grid[y*width + x] = numSteps;
		}
	}

	x = startX;
	y = startY;

	// Finding min distance for part1, min # of steps for part2
	auto minDist = numeric_limits<int>::max();
	auto minSteps = numeric_limits<int>::max();

	numSteps = 0;
	
	// Process second wire
	for (const auto &curLine : wire2) {
		for (auto i=0; i < curLine.steps; i++) {
			switch(curLine.dir) {
				case 'R': x++;
					break;
				case 'L': x--;
					break;
				case 'U': y--;
					break;
				case 'D': y++;
					break;
			}

			numSteps++;

			if (grid[y*width + x] > 0) {
				//cout << "INTERSECT AT " << x << ", " << y << '\n';
				int dist = abs(x - startX) + abs(y - startY);
				int steps = numSteps + grid[y*width + x];

				minDist = min(minDist, dist);
				minSteps = min(minSteps, steps);
			}
		}
	}

	cout << "Minimum Distance: " << minDist << '\n';
	cout << "Minimum Total Steps: " << minSteps << '\n';

	delete [] grid;

	return 0;
}
