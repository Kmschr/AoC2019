#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

constexpr float degToRad = 3.14159 / 180.0;
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

// see if ray has possibly passed an asteroid
// uses ray offset for direction
// and ray position for cell position
bool collides(float offX, float offY, float fracX, float fracY) {

	// Top right quadrant
	if (offX > 0.0f && offY < 0.0f)
		return fracX > 0.5f && fracY > 0.5f;
	// Bot right quadrant
	if (offX > 0.0f && offY > 0.0f)
		return fracX < 0.5f && fracY > 0.5f;
	// Bot left quadrant
	if (offX < 0.0f && offY > 0.0f)
		return fracX < 0.5f && fracY < 0.5f;
	// Top left quadrant
	if (offX < 0.0f && offY < 0.0f)
		return fracX > 0.5f && fracY < 0.5f;

	return false;
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

	// count how many asteroids to blast
	int numAsteroids = 0;
	for (x=0; x < width; x++)
		for (y=0; y < height; y++)
			if (hiddenField[x][y] == 'H' || hiddenField[x][y] == '#')
				numAsteroids++;

	int numBlasted = 0;
	float rayAngle = 270.0f;
	float angleStep = 0.05f;
	float traversed = 0.0f;
	int output;

	cout << numAsteroids << '\n';
	while (numBlasted < numAsteroids) {
		float rayDist = 0.0f;
		float rayStep = 0.1f;

		// ray direction vector
		float rayXDir = cos(rayAngle * degToRad);
		float rayYDir = sin(rayAngle * degToRad);

		//cout << "rayDir: " << rayXDir << ", " << rayYDir << '\n';

		x = maxX;
		y = maxY;
		while (inBounds(x, y)) {
			// ray offset vector (Y is inverse)
			float offX = rayXDir * rayDist;
			float offY = rayYDir * rayDist;

			//cout << "offset: " << offX << ", " << offY << '\n';

			// get final ray vector
			float rayX = (static_cast<float>(maxX) + 0.5) + offX;
			float rayY = (static_cast<float>(maxY) + 0.5) + offY;

			//cout << "ray: " << rayX << ", " << rayY << '\n';

			double intpartX;
			double intpartY;

			// determine if it's possible we've passed the asteroid in a cell yet
			float fracX = modf(rayX, &intpartX);
			float fracY = modf(rayY, &intpartY);

			x = static_cast<int>(intpartX);
			y = static_cast<int>(intpartY);

			//cout << "pos: " << x << ", " << y << '\n';

			rayDist += rayStep;

			if (!collides(offX, offY, fracX, fracY))
				continue;

			if (!inBounds(x, y))
				break;

			if (hiddenField[x][y] == '#') {
				hiddenField[x][y] = '1' + (numBlasted % 9);
				cout << (numBlasted+1) << " " << x << ", " << y << '\n';
				
				if (numBlasted+1 == 200)
					output = x * 100 + y;

				numBlasted++;

				// show blasting order and reset count
				if ((numBlasted+1) % 10 == 0) {
					cout << '\n';
					display(hiddenField);
					cout << '\n';

					for (x=0; x < width; x++)
						for (y=0; y < height; y++)
							if (hiddenField[x][y] > '0' && hiddenField[x][y] <= '9')
								hiddenField[x][y] = '.';
				}

				break;
			}

		}

		rayAngle += angleStep;
		traversed += angleStep;

		// recalculate hidden/visible asteroids every cycle
		if (traversed >= 360.0f) {
			for (x=0; x < width; x++)
				for (y=0; y < height; y++)
					if (hiddenField[x][y] == 'H')
						hiddenField[x][y] = '#';

			cout << "RECALCULATING HIDDEN" << '\n';
			display(hiddenField);
			cout << '\n';
			hideAsteroids(hiddenField, maxX, maxY);
			display(hiddenField);
			cout << '\n';
			cout << "MADE NEW HIDDEN" << '\n';
			traversed = 0.0f;
		}
	}

	display(hiddenField);
	cout << '\n' << output << '\n';

	return 0;
}
