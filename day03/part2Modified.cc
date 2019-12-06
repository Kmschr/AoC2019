#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <vector>
using namespace std;

struct vec2d {
	int x, y;
};

// Read a single wire from a stream
void readNextWire(ifstream &in, vector<pair<vec2d, vec2d>> &wire) {
	// Read line
	string line;
	getline(in, line);

	// Read moves within line
	stringstream ss(line);
	string move;

	vec2d pos = {0, 0};
	while (getline(ss, move, ',')) {
		// extract direction first
		char dir = move[0];
		move[0] = ' ';

		// convert dist string to int
		stringstream conv(move);
		int n;
		conv >> n;

		vec2d p1 = pos; 
		if (dir == 'R')
			pos.x += n;
		if (dir == 'L')
			pos.x -= n;

		wire.push_back(pair<char, int>(dir, n));
	}
}

int main() {
	ifstream in("input");

	vector<pair<char,int>> wire1;
	vector<pair<char,int>> wire2;

	readNextWire(in, wire1);
	readNextWire(in, wire2);

	// origin of wires
	int x_i = 10'000;
	int y_i = 10'000;

	int x = x_i;
	int y = y_i;

	int numSteps = 0;

	// process first wire
	for (auto move : wire1) {
		for (int i=0; i < move.second; ++i) {
			++numSteps;
			if (move.first == 'R')
				++x;
			else if (move.first == 'L')
				--x;
			else if (move.first == 'U')
				++y;
			else if (move.first == 'D')
				--y;
			if (grid[x][y] == 0)
				grid[x][y] = numSteps;
		}
	}

	x = x_i;
	y = y_i;
	int minSteps = 100000;

	numSteps = 0;
	
	// process second wire
	for (auto move : wire2) {
		for (int i=0; i < move.second; ++i) {
			++numSteps;
			if (move.first == 'R')
				++x;
			else if (move.first == 'L')
				--x;
			else if (move.first == 'U')
				++y;
			else if (move.first == 'D')
				--y;
			if (grid[x][y] > 0) {
				cout << "INTERSECT AT " << x << ", " << y << '\n';
				int steps = numSteps + grid[x][y];
				if (steps < minSteps)
					minSteps = steps;
			}
		}
	}

	cout << minSteps << '\n';

	for (int i=0; i < rows; ++i)
		delete [] grid[i];
	delete [] grid;

	return 0;
}
