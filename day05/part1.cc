#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

constexpr bool DEBUG = false;

int RUN(vector<int> &MEM) {
	int PC = 0;

	while (PC < MEM.size()) {
		// FETCH
		int OPCODE = MEM[PC] % 100;
		int MODES = MEM[PC] / 100;

		// HALT
		if (OPCODE == 99)
			break;

		// ADD/MUL
		if (OPCODE == 1 || OPCODE == 2) {
			int S1 = MEM[PC+1]; // SOURCE 1
			int S2 = MEM[PC+2]; // SOURCE 2
			int D1 = MEM[PC+3]; // DEST 1

			// APPLY POSITION MODES
			if (MODES % 10 == 0)
				S1 = MEM[S1];
			if (MODES / 10 == 0)
				S2 = MEM[S2];

			// PERFORM ADD/MUL
			if (OPCODE == 1)
				MEM[D1] = S1 + S2;
			if (OPCODE == 2)
				MEM[D1] = S1 * S2;

			PC += 4;
			continue;
		} 
		
		// INPUT
		if (OPCODE == 3) {
			int D1 = MEM[PC+1];

			cin >> MEM[D1];

			PC += 2;
			continue;
		}

		// OUTPUT
		if (OPCODE == 4) {
			int S1 = MEM[PC+1];

			if (MODES % 10 == 0)
				S1 = MEM[S1];

			cout << S1 << '\n';
			PC += 2;
			continue;
		}

		cout << "UNSUPPORTED INSTRUCTION - PC: " << PC << '\n';
	}

	return MEM[0];
}

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

	RUN(mem);

	return 0;
}
