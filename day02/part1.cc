#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

int main() {
	ifstream in("input");

	vector<int> mem;

	string s;
	while (getline(in, s, ',')) {
		stringstream conv(s);
		int n;
		conv >> n;
		mem.push_back(n);
	}

	for (size_t i=0; i < mem.size(); i += 4) {
		int opcode = mem[i];
		if (opcode == 99)
			break;

		int S1 = mem[i+1];
		int S2 = mem[i+2];
		int D = mem[i+3];

		int val1 = mem[S1];
		int val2 = mem[S2];

		if (opcode == 1)
			mem[D] = val1 + val2;
		else if (opcode == 2)
			mem[D] = val1 * val2;
	}

	cout << mem[0] << '\n';

	return 0;
}
