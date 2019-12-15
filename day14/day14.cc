#include <algorithm>
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
	int count;
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
// example: split a string "10 FUEL" into int 10 and string "FUEL"
chemical parseChemical(const string &chem) {
	auto space = chem.find(' ', 1);
	int count = stoi(chem.substr(0, space));
	string name = chem.substr(space+1);

	return chemical({ name, count });
}

bool sameSign(int x, int y) {
	return (x >= 0) ^ (y < 0);
}

// put chemical into its own place in queue, or add to existing chemical
void queueInsert(vector<chemical> &chemQueue, chemical toAdd) {
	if (toAdd.count == 0)
		return;

	// if already in queue, add to count
	for (auto i=0; i < chemQueue.size(); i++) {
		if (chemQueue[i].name == toAdd.name && sameSign(chemQueue[i].count, toAdd.count)) {
			chemQueue[i].count += toAdd.count;
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
	return abs(chem.count) >= reaction.output.count;
}

// are any of the current reactions available stoichiometric?
size_t getStoichiometric(vector<chemical> chemQueue, map<string, react> &reactions) {
	for (auto i=0; i < chemQueue.size(); i++)
		if (stoichiometric(reactions[chemQueue[i].name], chemQueue[i]))
			return i;

	return -1;
}

size_t getOreCost(map<string, react> &reactions, const string &chem) {
	vector<chemical> chemQueue;

	// initialize queue w/ first reaction
	chemical startingChem = { chem, 1 };
	chemQueue.push_back(startingChem);

	vector<chemical> wasted;

	auto ore = 0;
	while (!chemQueue.empty()) {
		//printQueue(chemQueue);

		// pop chemical off of queue
		auto nextChemical = getStoichiometric(chemQueue, reactions);
		chemical curChem;
		bool stoich = true;

		if (nextChemical == -1) {
			curChem = chemQueue.back();
			chemQueue.pop_back();
			stoich = false;
		} else {
			curChem = chemQueue[nextChemical];
			chemQueue.erase(chemQueue.begin() + nextChemical);
		}
		
		// get reaction for current chemical
		react curReact = reactions[curChem.name];

		// do different things depending on if we met conditions for a stoichiometric reaction
		if (!stoich) {
			// check if we wasted chemical when we mightve shouldnt have, and recover from that possibility
			bool stopEarly = false;
			for (auto i=0; i < wasted.size(); i++) {
				if (wasted[i].name == curChem.name && sameSign(wasted[i].count, curChem.count)) {
					// re-add wasted element to queue
					int newCount = wasted[i].count + curChem.count;
					chemical toAdd = { curChem.name, newCount };

					cout << '\n';
					printQueue(chemQueue);
					//printQueue(wasted);
					cout << "Current: " << curChem << '\n';
					cout << "Wasted: " << wasted[i] << '\n';
					cout << "New: " << toAdd << '\n';

					// still not enough for a reaction or just as much, so dont actually re-add to queue
					if (newCount > 0 && newCount <= curReact.output.count) {
						cout << "Still Wasted" << '\n';
						wasted[i].count = newCount;
						stopEarly = true;
						continue;
					}

					queueInsert(chemQueue, toAdd);

					// undo by adding negative chemicals, dirty trick
					for (auto inputChem : curReact.inputs) {
						if (inputChem.name == "ORE") {
							ore -= inputChem.count;
							continue;
						}

						chemical negChem = { inputChem.name, -inputChem.count };
						queueInsert(chemQueue, negChem);
					}

					stopEarly = true;

					// remove chemical from wasted list
					wasted.erase(wasted.begin() + i);
					printQueue(chemQueue);
					//printQueue(wasted);
				}
			}

			// we might not have to waste this chemical anymore
			if (stopEarly)
				continue;

			// have to waste some of the chemical
			wasted.push_back(curChem);

			// handle negative reactions
			int modifier = 1;
			if (curChem.count < 0)
				modifier = -1;

			for (auto inputChem : curReact.inputs) {
				inputChem.count *= modifier;

				if (inputChem.name == "ORE") {
					ore += inputChem.count;
					continue;
				}

				queueInsert(chemQueue, inputChem);
			}
		} else {
			int newCount = abs(curChem.count) - curReact.output.count;

			int modifier = 1;
			if (curChem.count < 0)
				modifier = -1;

			// add remainder back to queue
			if (newCount != 0) {
				chemical toAdd = { curChem.name, newCount*modifier };
				queueInsert(chemQueue, toAdd);
			}

			// add reaction elements to queue
			for (const auto &inputChem : curReact.inputs) {
				if (inputChem.name == "ORE") {
					ore += inputChem.count*modifier;
					continue;
				}

				chemical toAdd = { inputChem.name, inputChem.count*modifier };
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
	part1("input");

	return 0;
}