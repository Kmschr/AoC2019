#include "intcode.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

using namespace std;

constexpr int width = 100;
constexpr int height = 100;

// create portable bitmap file
void makePBM(int (&image)[width * height], string filename) {
	ofstream out(filename);

	out << "P1" << '\n';
	out << width << " " << height << '\n';
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++)
			out << image[y * width + x];
		out << '\n';
	}
}

void paintShip(INTCODE robot, int start) {
	// initialize hull
	int hull[width * height];
	bool painted[width * height];
	for (int i = 0; i < width*height; i++) {
		hull[i] = 0;
		painted[i] = false;
	}

	// starting point
	int x = width/2;
	int y = height/2;
	int dir = 0; // 0 == UP, 1 == RIGHT, 2 == DOWN, 3 == LEFT

	int statusCode = 2;

	// set starting point to white
	hull[y * width + x] = start;

	while (statusCode != 0) {
		//cout << x << ", " << y << '\n';

		if (x < 0 || x >= width || y < 0 || y >= height) {
			cerr << "WENT OUT OF BOUNDS, INCREASE SIZE" << '\n';
			break;
		}

		string panel = to_string(hull[y * width + x]);
		istringstream iss(panel);
		ostringstream oss;

		//cout << "INPUT: " << panel << '\n';

		statusCode = robot.RUN(iss, oss);

		string out = oss.str();

		//cout << "OUTPUT: " << out << '\n';

		int color = out[0] - '0';
		int turn = out[1] - '0';

		if (statusCode != 0) {
			hull[y * width + x] = color;
			painted[y * width + x] = true;
		}

		if (turn == 0)
			dir--;
		else
			dir++;

		// wrap directions
		if (dir == -1)
			dir = 3;
		else if (dir == 4)
			dir = 0;

		// move forward
		if (dir == 0)
			y--;
		else if (dir == 1)
			x++;
		else if (dir == 2)
			y++;
		else if (dir == 3)
			x--;

		//cout << '\n';
	}

	int numPainted = 0;
	for (int i=0; i < width * height; i++)
		if (painted[i])
			numPainted++;

	cout << "Start: " << start << '\n';
	cout << "Panels Painted: " << numPainted << '\n';

	makePBM(hull, "hull" + to_string(start + 1) + ".pbm");
}

int main() {
	INTCODE paintingRobot("input");

	paintShip(paintingRobot, 0);
	paintShip(paintingRobot, 1);

	return 0;
}
