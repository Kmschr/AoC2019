#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <math.h>

using namespace std;

constexpr float degToRad = 3.14159f / 180.0f;
constexpr size_t width = 34;
constexpr size_t height = 34;

int gcdCalc(int u, int v) {
	while (v != 0) {
		int r = u % v;
		u = v;
		v = r;
	}
	return u;
}

void simplifySlope(int &x, int &y) {
	int gcd = gcdCalc(abs(x), abs(y));
	x /= gcd;
	y /= gcd;
}

bool inBounds(int x, int y) {
	return !(x < 0 || x >= width || y < 0 || y >= height);
}

template <size_t size_x=width, size_t size_y=height>
int hideAsteroids(char (&field)[size_x][size_y], int x, int y) {
	// Start by assuming all in view
	int numInView = width*height;

	field[x][y] = 'X';

	for (size_t i=0; i < size_x; i++) {
		for (size_t j=0; j < size_y; j++) {
			// ignore current asteroid, empty space, and hidden asteroids
			if (field[i][j] != '#') {
				if (field[i][j] != 'H')
					numInView--;
				continue;
			}

			// get step size for exact path
			int dx = i - x;
			int dy = j - y;
			simplifySlope(dx, dy);

			int u = i + dx;
			int v = j + dy;
			while (inBounds(u, v)) {
				if (field[u][v] == '#') {
					//cout << "Blocked by " << i << ", " << j << '\n';
					//cout << "Slope " << dx << ", " << dy << '\n';
					//cout << "Can't see " << u << ", " << v << '\n';
					field[u][v] = 'H';
					numInView--;
				}

				u += dx;
				v += dy;
			}
		}
	}

	return numInView;
}

template <size_t size_x=width, size_t size_y=height>
void display(char (&field)[size_x][size_y]) {
	for (size_t y=0; y < size_y; y++) {
		for (size_t x=0; x < size_x; x++)
			cout << field[x][y];
		cout << '\n';
	}
}

int main() {
	ifstream in("input");
	char field[width][height];

	size_t x=0;
	size_t y=0;

	// Read in asteroids
	string line;
	while (getline(in, line)) {
		x=0;
		for (char c : line) {
			field[x][y] = c;
			x++;
		}
		y++;
	}

	display(field);
	cout << '\n';

	char hiddenField[width][height];
	int maxViewed = 0;
	size_t maxX;
	size_t maxY;

	// For each asteroid parse every other asteroid
	for (x=0; x < width; x++) {
		for (y=0; y < height; y++) {
			if (field[x][y] != '#')
				continue;

			// Copy field
			char testField[width][height];
			copy(&field[0][0], &field[0][0]+width*height, &testField[0][0]);

			int viewed = hideAsteroids(testField, x, y);

			if (viewed > maxViewed) {
				copy(&testField[0][0], &testField[0][0]+width*height, &hiddenField[0][0]);
				maxViewed = viewed;
				maxX = x;
				maxY = y;
			}
		}
	}

	display(hiddenField);

	cout << maxX << ", " << maxY << '\n';
	cout << maxViewed << '\n';

	return 0;
}
