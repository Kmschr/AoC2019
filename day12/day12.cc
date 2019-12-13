#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

typedef long long int64;
using namespace std;

struct vec3d {
	int x = 0;
	int y = 0;
	int z = 0;

	bool operator==(const vec3d &o) {
		return x == o.x && y == o.y && z == o.z;
	}
};

struct moon {
	vec3d pos;
	vec3d vel;

	void gravitate(const moon &other) {
		if (pos.x > other.pos.x)
			vel.x--;
		else if (pos.x < other.pos.x)
			vel.x++;

		if (pos.y > other.pos.y)
			vel.y--;
		else if (pos.y < other.pos.y)
			vel.y++;

		if (pos.z > other.pos.z)
			vel.z--;
		else if (pos.z < other.pos.z)
			vel.z++;
	}

	void update() {
		pos.x += vel.x;
		pos.y += vel.y;
		pos.z += vel.z;
	}

	int potential() {
		return abs(pos.x) + abs(pos.y) + abs(pos.z);
	}

	int kinetic() {
		return abs(vel.x) + abs(vel.y) + abs(vel.z);
	}

	bool operator==(const moon &o) {
		return pos == o.pos && vel == o.vel;
	}
};

ostream &operator<<(ostream &stream, const vec3d &v) {
	return stream << "<x=" << v.x << ", y=" << v.y << ", z=" << v.z << ">";
}

ostream &operator<<(ostream &stream, const moon &m) {
	return stream << "pos=" << m.pos << ", vel=" << m.vel;
}

vec3d parseCoords(string line) {
	vec3d coords;

	int xStart = line.find("x") + 2;
	int xEnd = line.find(",");
	string x = line.substr(xStart, xEnd - xStart);

	line = line.substr(xEnd+1);
	int yStart = line.find("y") + 2;
	int yEnd = line.find(",");
	string y = line.substr(yStart, yEnd - yStart);

	line = line.substr(yEnd+1);
	int zStart = line.find("z") + 2;
	int zEnd = line.find(">");
	string z = line.substr(zStart, zEnd - zStart);
	
	coords.x = stoi(x);
	coords.y = stoi(y);
	coords.z = stoi(z);

	return coords;
}

void stepMoons(vector<moon> &moons) {
	// apply gravity
	for (auto i=0; i < moons.size(); i++)
		for (auto j=0; j < moons.size(); j++)
			if (j != i) 
				moons[i].gravitate(moons[j]);

	// update position
	for (auto i=0; i < moons.size(); i++)
		moons[i].update();
}

void part1(vector<moon> moons) {
	for (int step=0; step < 1000; step++) {
		cout << "After " << step << " steps:" << '\n';
		for (moon m : moons)
			cout << m << '\n';
		cout << '\n';

		stepMoons(moons);
	}

	int totalEnergy = 0;

	// Get total energy
	for (auto i=0; i < moons.size(); i++)
		totalEnergy += moons[i].potential() * moons[i].kinetic();

	cout << "Total Energy: " << totalEnergy << '\n';
}

// Euclidean algorithm
int gcd(int64 a, int64 b) {
	while (b != 0) {
		int64 t = b;
		b = a % b;
		a = t;
	}

	return a;
}

// Least common multiple
int64 lcm(int64 a, int64 b) {
	return abs(a * b) / gcd(a, b);
}

bool missing(const vector<int> &periods) {
	for (auto i=0; i < periods.size(); i++)
		if (periods[i] == 0)
			return true;

	return false;
}

int64 getAxis(moon m, int a) {
	if (a==0)
		return m.pos.x;
	if (a==1)
		return m.pos.y;
	if (a==2)
		return m.pos.z;
	if (a==3)
		return m.vel.x;
	if (a==4)
		return m.vel.y;
	if (a==5)
		return m.vel.z;
	return 0;
}

bool axisMatches(const vector<moon> &moons, const vector<moon> &initial, int a) {
	for (auto i=0; i < moons.size(); i++)
		if (getAxis(moons[i], a) != getAxis(initial[i], a) || getAxis(moons[i], a + 3) != getAxis(initial[i], a+3))
			return false;

	return true;
}

void part2(vector<moon> moons) {	
	int steps = 0;

	vector<moon> initial = moons;
	vector<int> periods(3, 0);
	
	while (missing(periods)) {
		steps++;
		stepMoons(moons);
		
		// for every axis, check if that axis matches the original
		for (auto i=0; i < periods.size(); i++)
			if (periods[i] == 0 && axisMatches(moons, initial, i))
				periods[i] = steps;
	}

	int64 totalPeriod;

	cout << '\n' << "Filled times in " << steps << " steps" << endl;
	for (auto i=0; i < periods.size(); i++) {
		cout << periods[i] << '\n';
		if (i == 0)
			totalPeriod = periods[i];
		else
			totalPeriod = lcm(totalPeriod, periods[i]);
	}

	cout << totalPeriod << '\n';
}

int main() {
	ifstream in("input");

	vector<moon> moons;

	string line;
	while(getline(in, line))
		moons.push_back({ parseCoords(line) });

	part1(moons);
	part2(moons);

	return 0;
}