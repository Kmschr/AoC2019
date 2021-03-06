#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
using namespace std;

constexpr bool DEBUG = true;

void displayInstruction(int PC, string OPCODE, int MODES, vector<int> ARGS) {
	if (DEBUG)
		//cout << "0x" << hex << left << setw(4) << PC << dec;
		cout << left << setw(4) << PC;
		cout << setw(5) << OPCODE;
		cout << setw(2) << setfill('0') << MODES << setfill(' ');
		string delim = "  ";
		for (auto ARG : ARGS) {
			cout << delim << ARG;
			delim = ",";
		}
		cout << '\n';
}

int RUN(vector<int> &MEM) {
	int PC = 0; // PROGRAM COUNTER

	while (PC < MEM.size()) {
		// FETCH
		int OPCODE = MEM[PC] % 100;
		int MODES = MEM[PC] / 100;

		// HALT
		if (OPCODE == 99) {
			cout << "0x" << hex << left << setw(4) << PC << dec;
			cout << "HALT" << '\n';
			break;
		}

		vector<int> ARGS;

		// ADD/MUL
		if (OPCODE == 1 || OPCODE == 2) {
			int S1 = MEM[PC+1]; // SOURCE 1
			int S2 = MEM[PC+2]; // SOURCE 2
			int D1 = MEM[PC+3]; // DEST 1
			ARGS = { S1, S2, D1 };

			// APPLY POSITION MODES
			if (MODES % 10 == 0)
				S1 = MEM[S1];
			if (MODES / 10 == 0)
				S2 = MEM[S2];

			// PERFORM ADD/MUL
			if (OPCODE == 1) {
				MEM[D1] = S1 + S2;
				displayInstruction(PC, "ADD", MODES, ARGS);
			} else if (OPCODE == 2) {
				MEM[D1] = S1 * S2;
				displayInstruction(PC, "MUL", MODES, ARGS);
			}

			PC += 4;
			continue;
		} 
		
		// INPUT
		if (OPCODE == 3) {
			int D1 = MEM[PC+1];
			ARGS = { D1 };
			displayInstruction(PC, "IN", MODES, ARGS);

			cout << ">> ";
			cin >> MEM[D1];

			PC += 2;
			continue;
		}

		// OUTPUT
		if (OPCODE == 4) {
			int S1 = MEM[PC+1];
			ARGS = { S1 };
			displayInstruction(PC, "OUT", MODES, ARGS);

			if (MODES % 10 == 0)
				S1 = MEM[S1];

			cout << ">> ";
			cout << S1 << '\n';
			PC += 2;
			continue;
		}

		// JUMP
		if (OPCODE == 5 || OPCODE == 6) {
			int C1 = MEM[PC+1]; // CONDITIONAL
			int A1 = MEM[PC+2]; // JMP ADDRESS
			ARGS = { C1, A1 };

			if (MODES % 10 == 0)
				C1 = MEM[C1];
			if (MODES / 10 == 0)
				A1 = MEM[A1];

			if ((OPCODE == 5 && C1 != 0) || (OPCODE == 6 && C1 == 0))
				PC = A1;
			else
				PC += 3;

			if (OPCODE == 5)
				displayInstruction(PC, "JMPT", MODES, ARGS);
			else
				displayInstruction(PC, "JMPF", MODES, ARGS);
			continue;
		}

		// LESS/EQUAL
		if (OPCODE == 7 || OPCODE == 8) {
			int S1 = MEM[PC+1]; // SOURCE 1
			int S2 = MEM[PC+2]; // SOURCE 2
			int D1 = MEM[PC+3]; // DEST 1
			ARGS = { S1, S2, D1 };

			// APPLY POSITION MODES
			if (MODES % 10 == 0)
				S1 = MEM[S1];
			if (MODES / 10 == 0)
				S2 = MEM[S2];

			if ((OPCODE == 7 && S1 < S2) || (OPCODE == 8 && S1 == S2))
				MEM[D1] = 1;
			else
				MEM[D1] = 0;

			if (OPCODE == 7)
				displayInstruction(PC, "LESS", MODES, ARGS);
			else
				displayInstruction(PC, "EQL", MODES, ARGS);

			PC += 4;
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
