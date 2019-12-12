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
		cout << left << setw(5) << PC;
		cout << setw(6) << OPCODE;
		cout << setw(3) << right << setfill('0') << MODES << setfill(' ');
        cout << "  ";
        int n = 30;
        for (size_t i=0; i < ARGS.size(); i++) {
            int piece = n / 3;
            if (i != ARGS.size()-1)
                cout << setw(piece) << left << to_string(ARGS[i]) + ',';
            else
                cout << setw(piece) << left << to_string(ARGS[i]);
        }
        if (ARGS.size() != 3)
            cout << setw(n - (ARGS.size() * (n/3))) << ' ';
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

int run(long long *MEM, int &PC, long long &BASE, istringstream &in, ostringstream &out) {
	int numOutput = 0;

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

			// PERFORM ADD/MUL
			if (OPCODE == 1)
				MEM[D1] = S1 + S2;
			if (OPCODE == 2)
				MEM[D1] = S1 * S2;

            if (DEBUG) {
                if (OPCODE == 1)
                    if (S2 >= 0)
                        cout << "MEM[" << D1 << "] = " << S1 << " + " << S2 << '\n';
                    else
                        cout << "MEM[" << D1 << "] = " << S1 << " - " << -S2 << '\n';
                if (OPCODE == 2)
                    cout << "MEM[" << D1 << "] = " << S1 << " * " << S2 << '\n';
            }

			PC += 4;
			continue;
		} 
		
		// INPUT
		if (OPCODE == 3) {
		    int D1 = MEM[PC+1];
			ARGS = { D1 };
			displayInstruction(PC, "IN", MODES, ARGS);
            if (DEBUG)
                cout << "MEM[" << D1 << "] = INPUT" << '\n';

            applyModes(MEM, ARGS, MODES, BASE, 0);
            D1 = ARGS[0];

			//cout << ">> ";
			in >> MEM[D1];

            
			PC += 2;
			continue;
		}

		// OUTPUT
		if (OPCODE == 4) {
			long long S1 = MEM[PC+1];
			ARGS = { S1 };
			displayInstruction(PC, "OUT", MODES, ARGS);
            if (DEBUG)
                cout << '\n';

            applyModes(MEM, ARGS, MODES, BASE);
            S1 = ARGS[0];

			// print to stdout first, then ostringstream
			if (DEBUG)
				cout << ">> " << S1 << '\n';
			out << S1;

			PC += 2;

			numOutput++;
			if (numOutput == 2)
				return 2;

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

			if ((OPCODE == 5 && C1 != 0) || (OPCODE == 6 && C1 == 0))
				PC = A1;
			else
				PC += 3;
            
			if (DEBUG) {
				if (OPCODE == 5)
					cout << "if (" << C1 << " != 0) PC = " << A1 << '\n';
				else
					cout << "if (" << C1 << " == 0) PC = " << A1 << '\n';
			}

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
			if (OPCODE == 8)
				displayInstruction(PC, "EQL", MODES, ARGS);

            applyModes(MEM, ARGS, MODES, BASE, 2);
            S1 = ARGS[0];
            S2 = ARGS[1];
            D1 = ARGS[2];

			if ((OPCODE == 7 && S1 < S2) || (OPCODE == 8 && S1 == S2))
				MEM[D1] = 1;
			else
				MEM[D1] = 0;

			if (DEBUG) {
				if (OPCODE == 7)
					cout << "if (" << S1 << " < " << S2 << ") MEM[" << D1 << "] = 1" << '\n';
				if (OPCODE == 8)
					cout << "if (" << S1 << " == " << S2 << ") MEM[" << D1 << "] = 1" << '\n';
			}

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

            BASE += S1;

			if (DEBUG) {
				if (S1 >= 0)
					cout << "BASE += " << S1 << '\n';
				else
					cout << "BASE -= " << -S1 << '\n';
			}

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
    mem = new long long[10000]();
	long long base = 0;
	int pc = 0;

	string s;
    int i=0;
	while (getline(in, s, ',')) {
		stringstream conv(s);
		int n;
		conv >> n;
        mem[i] = n;
        ++i;
	}

	constexpr int width = 300;
	constexpr int height = 300;

	int hull[width][height];
	bool painted[width][height];
	for (int i=0; i < height; i++) {
		for (int j=0; j < width; j++) {
			hull[j][i] = 0;
			painted[j][i] = false;
		}
	}

	int x = width/2;
	int y = height/2;
	int dir = 0; // 0 == UP, 1 == RIGHT, 2 == DOWN, 3 == LEFT

	int code = 2;

	while (code != 0) {
		//cout << x << ", " << y << '\n';

		if (x < 0 || x >= width || y < 0 || y >= height) {
			cerr << "WENT OUT OF BOUNDS" << '\n';
			exit(1);
		}

		string panel = to_string(hull[x][y]);
		istringstream iss(panel);
		ostringstream oss;

		//cout << "INPUT: " << panel << '\n';

		code = run(mem, pc, base, iss, oss);

		string out = oss.str();

		//cout << "OUTPUT: " << out << '\n';

		int color = out[0] - '0';
		int turn = out[1] - '0';

		if (code != 0) {
			hull[x][y] = color;
			painted[x][y] = true;
		}

		if (turn == 0)
			dir--;
		else
			dir++;

		// wrap directions
		if (dir == -1)
			dir = 3;
		else if (dir == 4)
			dir = 0;

		// move forward
		if (dir == 0)
			y--;
		else if (dir == 1)
			x++;
		else if (dir == 2)
			y++;
		else if (dir == 3)
			x--;

		//cout << '\n';
	}

	cout << x << ", " << y << '\n';

	int numPainted = 0;
	for (int i=0; i < height; i++) 
		for (int j=0; j < width; j++)
			if (painted[i][j])
				numPainted++;

	cout << numPainted << '\n';

    delete[] mem;

	return 0;
}
