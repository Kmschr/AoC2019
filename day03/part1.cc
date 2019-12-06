#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <vector>
using namespace std;

// Read a single wire from a stream
void readNextWire(ifstream &in, vector<pair<char, int>> &wire) {
	// Read line
	string line;
	getline(in, line);

	// Read moves within line
	stringstream ss(line);
	string move;
	while (getline(ss, move, ',')) {
		// extract direction first
		char dir = move[0];
		move[0] = ' ';

		// convert dist string to int
		stringstream conv(move);
		int n;
		conv >> n;

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

	int rows = 20'000;
	int cols = 20'000;

	char** grid = new char*[rows];
	for (int i=0; i < rows; ++i)
		grid[i] = new char[cols];
	grid[x_i][y_i] = 'O';


	int x = x_i;
	int y = y_i;

	// process first wire
	for (auto move : wire1) {
		for (int i=0; i < move.second; ++i) {
			if (move.first == 'R')
				++x;
			else if (move.first == 'L')
				--x;
			else if (move.first == 'U')
				++y;
			else if (move.first == 'D')
				--y;
			if (grid[x][y] != 'O')
				grid[x][y] = '1';
		}
	}

	x = x_i;
	y = y_i;
	int minDist = 10000;
	
	// process second wire
	for (auto move : wire2) {
		for (int i=0; i < move.second; ++i) {
			if (move.first == 'R')
				++x;
			else if (move.first == 'L')
				--x;
			else if (move.first == 'U')
				++y;
			else if (move.first == 'D')
				--y;
			if (grid[x][y] == '1') {
				grid[x][y] = 'X';
				cout << "INTERSECT AT " << x << ", " << y << '\n';
				int dist = abs(x - x_i) + abs(y - y_i);
				if (dist < minDist)
					minDist = dist;
			}
		}
	}

	cout << minDist << '\n';

	for (int i=0; i < rows; ++i)
		delete [] grid[i];
	delete [] grid;

	return 0;
}
