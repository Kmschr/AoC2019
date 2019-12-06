#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

vector<string> getChain(const string &start, map<string, string> &orbits) {
	vector<string> chain;

	string next = orbits[start];

	while (orbits.find(next) != orbits.end()) {
		chain.push_back(next);
		next = orbits[next];
	}

	return chain;
}

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

	vector<string> santaChain = getChain("SAN", orbits);
	vector<string> youChain = getChain("YOU", orbits);

	for (auto i=0; i < santaChain.size(); ++i) {
		auto it = find(youChain.begin(), youChain.end(), santaChain[i]);
		if (it != youChain.end()) {
			auto dist = distance(youChain.begin(), it);	
			cout << i << '\n';
			cout << dist << '\n';
			cout << (i + dist) << '\n';
			exit(0);
		}
	}

	return 0;
}
