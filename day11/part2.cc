#include "intcode.h"
#include <fstream>
#include <sstream>
#include <string>
#include <map>

using namespace std;

int main() {
	ifstream in("input");

	map<INT64, INT64> mem;
	INT64 base = 0;
	INT64 pc = 0;

	// load memory
	string s;
	int i=0;
	while (getline(in, s, ',')) {
		stringstream conv(s);
		int n;
		conv >> n;
		mem[i] = n;
		++i;
	}

	constexpr int width = 200;
	constexpr int height = 200;

	// initialize hull
	int hull[width * height];
	for (int i = 0; i < width*height; i++)
		hull[i] = 0;

	// starting point
	int x = width/2;
	int y = height/2;
	int dir = 0; // 0 == UP, 1 == RIGHT, 2 == DOWN, 3 == LEFT

	int statusCode = 2;

	// set starting point to white
	hull[y * width + x] = 1;

	while (statusCode != 0) {
		cout << x << ", " << y << '\n';

		if (x < 0 || x >= width || y < 0 || y >= height) {
			cerr << "WENT OUT OF BOUNDS, INCREASE SIZE" << '\n';
			break;
		}

		string panel = to_string(hull[y * width + x]);
		istringstream iss(panel);
		ostringstream oss;

		cout << "INPUT: " << panel << '\n';

		statusCode = RUN(mem, pc, base, iss, oss);

		string out = oss.str();

		cout << "OUTPUT: " << out << '\n';

		int color = out[0] - '0';
		int turn = out[1] - '0';

		if (statusCode != 0)
			hull[y * width + x] = color;

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

		cout << '\n';
	}

	// make registration image
	ofstream out("reg.pbm");

	out << "P1" << '\n';
	out << width << " " << height << '\n';
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (hull[y * width + x] == 1)
				out << 0 << ' ';
			else
				out << 1 << ' ';
		}
		out << '\n';
	}

	return 0;
}
