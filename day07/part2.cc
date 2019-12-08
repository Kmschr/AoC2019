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

void displayInstruction(int PC, string OPCODE, int MODES, vector<int> ARGS) {
	if (DEBUG) {
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
}

bool run(vector<int> &MEM, int &PC, queue<int> &INPUT, int &OUTPUT) {
	while (PC < MEM.size()) {
		// FETCH
		int OPCODE = MEM[PC] % 100;
		int MODES = MEM[PC] / 100;

		// HALT
		if (OPCODE == 99) {
            if (DEBUG)
			    cout << left << setw(4) << PC << "HALT" << '\n';
            return false;
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

			//cout << ">> ";
			//cin >> MEM[D1];
            
            if (DEBUG)
                cout << ">> " << INPUT.front() << '\n';
            MEM[D1] = INPUT.front();
            INPUT.pop();

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

            if (DEBUG)
			    cout << ">> " << S1 << '\n';

            OUTPUT = S1;

			PC += 2;
            break;
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

	return true;
}

struct Amplifier {
    vector<int> mem;
    int pc = 0;
    int output;
};

bool runAmplifier(Amplifier &amp, queue<int> &input) {
    return run(amp.mem, amp.pc, input, amp.output); 
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

    int inputs[] = { 9, 8, 7, 6, 5 };
    sort(inputs, inputs+5);

    int maxSignal = 0;

    do {
        // Reset amplifiers
        Amplifier amps[5];
        for (auto &amp : amps)
            amp.mem = mem;

        int output = 0;
        bool running = true;
        bool start = true;

        while (running) {
            for (int i=0; i < 5; ++i) {
                queue<int> input;
                if (start)
                    input.push(inputs[i]);
                input.push(output);

                bool run = runAmplifier(amps[i], input);
                output = amps[i].output;

                if (!run)
                    running = false;
            }

            start = false;
        }

        if (output > maxSignal)
            maxSignal = output;

    } while (next_permutation(inputs, inputs+5));

    cout << maxSignal << '\n';

	return 0;
}
