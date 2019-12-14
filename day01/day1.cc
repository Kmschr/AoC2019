#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// mass to fuel conversion
int mtof(int mass) {
	return mass/3 - 2;
}

void part1(const string &filename) {
	ifstream in(filename);

	int fuel = 0;

	string line;
	while (getline(in, line)) {
		int mass = stoi(line);
		fuel += mtof(mass);
	}

	cout << "Fuel Required: " << fuel << '\n';
}

void part2(const string &filename) {
	ifstream in("input");

	int totalFuel = 0;

	string line;
	while (getline(in, line)) {
		int mass = stoi(line);

		while (mass > 0) {
			int fuel = mtof(mass);

			if (fuel > 0)
				totalFuel += fuel;

			mass = fuel;
		}
	}

	cout << "Fuel Required: " << totalFuel << '\n';
}

int main() {
	part1("input");
	part2("input");

	return 0;
}