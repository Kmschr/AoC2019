#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <queue>
#include <map>
using namespace std;

struct chemical {
	string name;
	size_t count;
};

// chemical ostream operator overload
ostream &operator<<(ostream &stream, const chemical &c) {
	return stream << c.count << " " << c.name;
}

// reaction definition
struct react {
	vector<chemical> inputs;
	chemical output;
};

// reaction ostream operator overload
ostream &operator<<(ostream &stream, const react &r) {
	for (auto i=0; i < r.inputs.size(); i++) {
		stream << r.inputs[i];
		if (i != r.inputs.size()-1)
			stream << ", ";
	}

	stream << " => " << r.output;
	return stream;
}

// splits chemical string into components
// example: split a string "10 FUEL" into size_t 10 and string "FUEL"
chemical parseChemical(const string &chem) {
	auto space = chem.find(' ', 1);
	size_t count = stoi(chem.substr(0, space));
	string name = chem.substr(space+1);

	return chemical({ name, count });
}

// put chemical into its own place in queue, or add to existing chemical
void queueInsert(vector<chemical> &chemQueue, chemical toAdd) {
	// if already in queue, add to count
	for (auto &chem : chemQueue) {
		if (chem.name == toAdd.name) {
			chem.count += toAdd.count;
			return;
		}
	}

	chemQueue.insert(chemQueue.begin(), toAdd);
}

// print queue in order from back to front
void printQueue(vector<chemical> &chemQueue) {
	cout << "<";
	for (auto i=0; i < chemQueue.size(); i++) {
		cout << chemQueue[i];
		if (i != chemQueue.size()-1)
			cout << ", ";
	}
	cout << ">" << '\n';
}

// do we have enough of chemical to form complete reaction
bool stoichiometric(const react &reaction, const chemical &chem) {
	return chem.count >= reaction.output.count;
}

// are any of the current reactions available stoichiometric?
bool anyStoichiometric(vector<chemical> chemQueue, map<string, react> &reactions) {
	for (const auto &chem : chemQueue)
		if (stoichiometric(reactions[chem.name], chem))
			return true;

	return false;
}

size_t getOreCost(map<string, react> &reactions, const string &chem) {
	vector<chemical> chemQueue;

	// initialize queue w/ first reaction
	chemical startingChem = { chem, 1 };
	chemQueue.push_back(startingChem);

	auto ore = 0;
	while (!chemQueue.empty()) {
		//printQueue(chemQueue);

		// pop chemical off of queue
		chemical current = chemQueue.back();
		chemQueue.pop_back();

		// get reaction for current chemical
		react curReact = reactions[current.name];

		// do different things depending on if we met conditions for a stoichiometric reaction
		if (!stoichiometric(curReact, current)) {
			if (anyStoichiometric(chemQueue, reactions)) {
				// put back into queue, process later
				queueInsert(chemQueue, current);
			} else {
				// have to waste some of the chemical
				for (auto chem : curReact.inputs) {
					//cout << chem << " => " << curReact.output << '\n';
					if (chem.name == "ORE") {
						ore += chem.count;
						continue;
					}

					queueInsert(chemQueue, chem);
				}
			}
		} else {
			size_t newCount = current.count % curReact.output.count;
			size_t numReactions = current.count / curReact.output.count;

			// add remainder back to queue
			if (newCount != 0) {
				chemical toAdd = { current.name, newCount };
				queueInsert(chemQueue, toAdd);
			}

			// add reaction elements to queue
			for (const auto &chem : curReact.inputs) {
				//cout << chem.count * numReactions << " " << chem.name << " => " << current.count - newCount << " " << current.name << '\n';
				if (chem.name == "ORE") {
					ore += chem.count*numReactions;
					continue;
				}

				chemical toAdd = { chem.name, chem.count*numReactions };
				queueInsert(chemQueue, toAdd);
			}
		}
	}

	return ore;
}

void part1(const string &filename) {
	ifstream in(filename);

	// collection of reactions, accessible by reaction output chemical name
	map<string, react> reactions;

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

	auto oreCount = getOreCost(reactions, "FUEL");
	cout << oreCount << '\n';
}

int main() {
	part1("test");

	return 0;
}