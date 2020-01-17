#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <deque>
using namespace std;

typedef unsigned long long u64;

struct chemical {
	string name;
	u64 count;
};

// reaction definition
struct react {
	vector<chemical> inputs;
	chemical output;
};

// splits chemical string into components
// example: split a string "10 FUEL" into u64 10 and string "FUEL"
chemical parseChemical(const string &chem) {
	auto space = chem.find(' ', 1);
	u64 count = stoull(chem.substr(0, space));
	string name = chem.substr(space+1);

	return chemical({ name, count });
}

u64 oreCost(unordered_map<string, react> &reactions, unordered_map<string, chemical> &extraChems, u64 quantity) {
	deque<chemical> neededChems;
	neededChems.push_back({"FUEL", quantity});

	u64 oreCount = 0;

	while (!neededChems.empty()) {
		chemical &curChem = neededChems.front();
		neededChems.pop_front();

		if (curChem.name == "ORE") {
			chemical &extraOre = extraChems["ORE"];
			u64 extraUsed = min(curChem.count, extraOre.count);
			curChem.count -= extraUsed;
			oreCount += curChem.count;
		} else {
			chemical &extra = extraChems[curChem.name];
			u64 extraUsed = min(curChem.count, extra.count);
			curChem.count -= extraUsed;
			extra.count -= extraUsed;

			if (curChem.count > 0) {
				react &reaction = reactions[curChem.name];
				u64 numReactions = (curChem.count - 1) / reaction.output.count + 1;
				extra.count = reaction.output.count * numReactions - curChem.count;

				for (auto &inputChem : reaction.inputs) {
					chemical toAdd = { inputChem.name, inputChem.count * numReactions };
					neededChems.push_back(toAdd);
				}
			}
		}
	}

	return oreCount;
}

unordered_map<string, react> loadReactions(const string &filename) {
	ifstream in(filename);

	unordered_map<string, react> reactions;

	// parse reactions
	string reaction;
	while (getline(in, reaction)) {
		react toAdd;

		// divide line into inputs/output (left hand side, right hand side)
		auto middle = reaction.find(" => ");
		string LHS = reaction.substr(0, middle);
		string RHS = reaction.substr(middle+4);

		// parse inputs
		string chem;
		stringstream inputs(LHS);
		while (getline(inputs, chem, ','))
			toAdd.inputs.push_back(parseChemical(chem));

		// parse output
		toAdd.output = parseChemical(RHS);

		reactions[toAdd.output.name] = toAdd;
	}

	return reactions;
}

void part1(unordered_map<string, react> reactions) {
	unordered_map<string, chemical> extraChems;
	cout << oreCost(reactions, extraChems, 1) << '\n';
}

void part2(unordered_map<string, react> reactions) {
	unordered_map<string, chemical> extraChems;

	u64 orePerFuel = oreCost(reactions, extraChems, 1);
	u64 fuelProduced = 1;

	if (extraChems.find("FUEL") != extraChems.end()) {
		fuelProduced += extraChems["FUEL"].count;
		extraChems.erase("FUEL");
	}

	u64 producedFuelMultiplier = fuelProduced;

	extraChems["ORE"].count = 1'000'000'000'000 - orePerFuel;

	while (true) {
		u64 producableFuel = max(1ULL, extraChems["ORE"].count / orePerFuel * producedFuelMultiplier);
		u64 requiredOre = oreCost(reactions, extraChems, producableFuel);

		if (requiredOre != 0) {
			cout << fuelProduced << '\n';
			return;
		}

		fuelProduced += producableFuel;
	}
}

int main() {
	unordered_map<string, react> reactions(loadReactions("input"));
	
	part1(reactions);
	part2(reactions);

	return 0;
}