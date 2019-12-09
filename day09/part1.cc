#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <iomanip>
#include <algorithm>
using namespace std;

constexpr bool DEBUG = false;

void displayInstruction(int PC, string OPCODE, int MODES, vector<long long> ARGS) {
	if (DEBUG) {
		//cout << "0x" << hex << left << setw(4) << PC << dec;
		cout << left << setw(4) << PC;
		cout << setw(5) << OPCODE;
		cout << setw(3) << right << setfill('0') << MODES << setfill(' ') << left;
		string delim = "  ";
		for (auto ARG : ARGS) {
			cout << delim << ARG;
			delim = ",";
		}
		cout << '\n';
    }
}

void applyModes(long long *MEM, vector<long long> &ARGS, int MODES, long long BASE, int DEST=-1) {
    for (size_t i=0; i < ARGS.size(); ++i) {
        int MODE = MODES % 10;

        // POSITION MODE
        if (MODE == 0)
            ARGS[i] = ARGS[i];

        // RELATIVE MODE
        if (MODE == 2)
            ARGS[i] = BASE + ARGS[i];

        // ONLY FETCH FROM MEMORY IF SOURCE
        if (i != DEST && MODE != 1)
            ARGS[i] = MEM[ARGS[i]];

        MODES /= 10;
    }
}

int run(long long *MEM) {
    int PC = 0;
    long long BASE = 0;

	while (PC >= 0) {
		// FETCH
		int OPCODE = MEM[PC] % 100;
		int MODES = MEM[PC] / 100;

		// HALT
		if (OPCODE == 99) {
            if (DEBUG)
			    cout << left << setw(4) << PC << "HALT" << '\n';
            return 0;
		}

		vector<long long> ARGS;

		// ADD/MUL
		if (OPCODE == 1 || OPCODE == 2) {
			long long S1 = MEM[PC+1]; // SOURCE 1
		    long long S2 = MEM[PC+2]; // SOURCE 2
			int D1 = MEM[PC+3]; // DEST 1
			ARGS = { S1, S2, D1 };

            if (OPCODE == 1)
				displayInstruction(PC, "ADD", MODES, ARGS);
            if (OPCODE == 2)
				displayInstruction(PC, "MUL", MODES, ARGS);

            applyModes(MEM, ARGS, MODES, BASE, 2);
            S1 = ARGS[0];
            S2 = ARGS[1];
            D1 = ARGS[2];

            if (OPCODE == 1)
				displayInstruction(PC, "ADD", MODES, ARGS);
            if (OPCODE == 2)
				displayInstruction(PC, "MUL", MODES, ARGS);

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
			ARGS = { D1 };
			displayInstruction(PC, "IN", MODES, ARGS);

            applyModes(MEM, ARGS, MODES, BASE, 0);
            D1 = ARGS[0];

			displayInstruction(PC, "IN", MODES, ARGS);

			cout << ">> ";
			cin >> MEM[D1];
            
			PC += 2;
			continue;
		}

		// OUTPUT
		if (OPCODE == 4) {
			long long S1 = MEM[PC+1];
			ARGS = { S1 };
			displayInstruction(PC, "OUT", MODES, ARGS);

            applyModes(MEM, ARGS, MODES, BASE);
            S1 = ARGS[0];

			cout << ">> " << S1 << '\n';

			PC += 2;
            continue;
		}

		// JUMP
		if (OPCODE == 5 || OPCODE == 6) {
			long long C1 = MEM[PC+1]; // CONDITIONAL
			int A1 = MEM[PC+2]; // JMP ADDRESS
			ARGS = { C1, A1 };

			if (OPCODE == 5)
				displayInstruction(PC, "JMPT", MODES, ARGS);
			else
				displayInstruction(PC, "JMPF", MODES, ARGS);

            applyModes(MEM, ARGS, MODES, BASE);
            C1 = ARGS[0];
            A1 = ARGS[1];

			if (OPCODE == 5)
				displayInstruction(PC, "JMPT", MODES, ARGS);
			else
				displayInstruction(PC, "JMPF", MODES, ARGS);

			if ((OPCODE == 5 && C1 != 0) || (OPCODE == 6 && C1 == 0))
				PC = A1;
			else
				PC += 3;

			continue;
		}

		// LESS/EQUAL
		if (OPCODE == 7 || OPCODE == 8) {
			long long S1 = MEM[PC+1]; // SOURCE 1
			long long S2 = MEM[PC+2]; // SOURCE 2
		    int D1 = MEM[PC+3]; // DEST 1
			ARGS = { S1, S2, D1 };

			if (OPCODE == 7)
				displayInstruction(PC, "LESS", MODES, ARGS);
			else
				displayInstruction(PC, "EQL", MODES, ARGS);

            applyModes(MEM, ARGS, MODES, BASE, 2);
            S1 = ARGS[0];
            S2 = ARGS[1];
            D1 = ARGS[2];

			if (OPCODE == 7)
				displayInstruction(PC, "LESS", MODES, ARGS);
			else
				displayInstruction(PC, "EQL", MODES, ARGS);

			if ((OPCODE == 7 && S1 < S2) || (OPCODE == 8 && S1 == S2))
				MEM[D1] = 1;
			else
				MEM[D1] = 0;

			PC += 4;
			continue;
		}

        // BASE ADJUST
        if (OPCODE == 9) {
            long long S1 = MEM[PC+1]; // SOURCE
            ARGS = { S1 };

			displayInstruction(PC, "BASE", MODES, ARGS);

            applyModes(MEM, ARGS, MODES, BASE);
            S1 = ARGS[0];

			displayInstruction(PC, "BASE", MODES, ARGS);

            BASE += S1;

            PC += 2;
            continue;
        }

		cout << "UNSUPPORTED INSTRUCTION - PC: " << PC << '\n';
        PC = -1;
	}

	return 1;
}

int main() {
	ifstream in("input");

	long long *mem;
    mem = new long long[64000]();

	string s;
    int i=0;
	while (getline(in, s, ',')) {
		stringstream conv(s);
		int n;
		conv >> n;
        mem[i] = n;
        ++i;
	}

    run(mem);

    delete[] mem;

	return 0;
}
