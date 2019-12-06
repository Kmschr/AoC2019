#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>
using namespace std;

int main() {
	map<string, string> orbits;

	ifstream in("input");

	// read in all the orbits
	string line;
	while (getline(in, line)) {
		auto split = line.find(")");
		string center = line.substr(0, split);
		string object = line.substr(split+1);
		orbits.insert(pair<string, string>(object, center));
	}

	int direct = 0;
	int indirect = 0;

	// find total number of orbits
	for (const auto &orbit : orbits) {
		string next = orbit.second;
		auto chain = orbits.find(next);	
		while (chain != orbits.end()) {
			indirect++;
			next = orbits[next];
			chain = orbits.find(next);
		}
		direct++;	
	}

	cout << "Direct: " << direct << '\n';
	cout << "Indirect: " << indirect << '\n';
	cout << "Total: " << (direct+indirect) << '\n';

	return 0;
}
