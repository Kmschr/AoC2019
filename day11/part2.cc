#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <queue>
#include <iomanip>
#include <algorithm>
using namespace std;

typedef long long INT64;
constexpr bool DEBUG = false;

void DISPLAY_INSTRUCTION(INT64 PC, string OPCODE, int MODES, const vector<INT64*> &ARGS) {
	if (!DEBUG)
		return;

	size_t NUM_ARGS = ARGS.size();
	constexpr size_t MAX_ARGS = 3;

	// DISPLAY INSTRUCTION
	cout << setw(5) << left << PC;
	cout << setw(6) << OPCODE;
	cout << setw(3) << right << setfill('0') << MODES << setfill(' ');
	cout << "  ";

	// DISPLAY ARGUMENTS
	size_t ARG_SPACE = 30 / MAX_ARGS;
	for (size_t i=0; i < NUM_ARGS; i++) {
		if (i != NUM_ARGS-1)
			cout << setw(ARG_SPACE) << left << to_string(*ARGS[i]) + ',';
		else
			cout << setw(ARG_SPACE) << left << to_string(*ARGS[i]);
	}

	// FILL EMPTY ARGUMENT SPACE
	if (NUM_ARGS != MAX_ARGS)
		cout << setw(ARG_SPACE * (MAX_ARGS - NUM_ARGS)) << ' ';
}

void APPLY_MODES(map<INT64, INT64> &MEM, vector<INT64*> &ARGS, int MODES, INT64 BASE, size_t DEST=50) {
	for (size_t i=0; i < ARGS.size(); ++i) {
		// GET LAST DIGIT OF MODES
		int MODE = MODES % 10;

		// POSITION MODE
		if (MODE == 0)
			*ARGS[i] = *ARGS[i];

		// RELATIVE MODE
		if (MODE == 2)
			*ARGS[i] = BASE + *ARGS[i];

		// ONLY FETCH FROM MEMORY IF NOT A DESTINATION ADDRESS
		if (i != DEST && MODE != 1)
			*ARGS[i] = MEM[*ARGS[i]];

		// REMOVE LAST DIGIT FROM MODES
		MODES /= 10;
	}
}

int run(map<INT64, INT64> &MEM, INT64 &PC, INT64 &BASE, istringstream &IN, ostringstream &OUT) {
	int NUM_OUT = 0;

	while (PC >= 0) {
		// DECODE INSTRUCTION
		int OPCODE = MEM[PC] % 100;
		int MODES = MEM[PC] / 100;
		
		vector<INT64*> ARGS;

		// HALT
		if (OPCODE == 99) {
			DISPLAY_INSTRUCTION(PC, "HALT", MODES, ARGS);
			return 0;
		}

		// ADD/MUL
		if (OPCODE == 1 || OPCODE == 2) {
			INT64 S1 = MEM[PC+1]; // SOURCE 1
			INT64 S2 = MEM[PC+2]; // SOURCE 2
			INT64 D1 = MEM[PC+3]; // DEST
			ARGS.push_back(&S1);
			ARGS.push_back(&S2);
			ARGS.push_back(&D1);

			if (OPCODE == 1)
				DISPLAY_INSTRUCTION(PC, "ADD", MODES, ARGS);
			if (OPCODE == 2)
				DISPLAY_INSTRUCTION(PC, "MUL", MODES, ARGS);

			APPLY_MODES(MEM, ARGS, MODES, BASE, 2);

			// PERFORM ADD/MUL
			if (OPCODE == 1)
				MEM[D1] = S1 + S2;
			if (OPCODE == 2)
				MEM[D1] = S1 * S2;

			// SHOW PSEUDO-CODE
			if (DEBUG) {
				if (OPCODE == 1)
					cout << "MEM[" << D1 << "] = " << S1 << " + " << S2 << '\n';
				if (OPCODE == 2)
					cout << "MEM[" << D1 << "] = " << S1 << " * " << S2 << '\n';
			}

			PC += 4;
		} 
		
		// INPUT
		if (OPCODE == 3) {
			INT64 D1 = MEM[PC+1]; // DEST
			ARGS.push_back(&D1);

			DISPLAY_INSTRUCTION(PC, "IN", MODES, ARGS);
			if (DEBUG)
				cout << "MEM[" << D1 << "] = INPUT" << '\n';

			//cout << ">> ";
			IN >> MEM[D1];
			
			PC += 2;
		}

		// OUTPUT
		if (OPCODE == 4) {
			INT64 S1 = MEM[PC+1]; // SOURCE
			ARGS.push_back(&S1);

			DISPLAY_INSTRUCTION(PC, "OUT", MODES, ARGS);

			APPLY_MODES(MEM, ARGS, MODES, BASE);

			// print to stdout first, then ostringstream
			if (DEBUG)
				cout << '\n' << ">> " << S1 << '\n';
			OUT << S1;

			PC += 2;

			if (++NUM_OUT == 2)
				return NUM_OUT;
		}

		// JUMP
		if (OPCODE == 5 || OPCODE == 6) {
			INT64 C1 = MEM[PC+1]; // CONDITIONAL
			INT64 A1 = MEM[PC+2]; // JMP ADDRESS
			ARGS.push_back(&C1);
			ARGS.push_back(&A1);

			if (OPCODE == 5)
				DISPLAY_INSTRUCTION(PC, "JMPT", MODES, ARGS);
			if (OPCODE == 6)
				DISPLAY_INSTRUCTION(PC, "JMPF", MODES, ARGS);

			APPLY_MODES(MEM, ARGS, MODES, BASE);

			if ((OPCODE == 5 && C1 != 0) || (OPCODE == 6 && C1 == 0))
				PC = A1;
			else
				PC += 3;
			
			if (DEBUG) {
				if (OPCODE == 5)
					cout << "if (" << C1 << " != 0) PC = " << A1 << '\n';
				if (OPCODE == 6)
					cout << "if (" << C1 << " == 0) PC = " << A1 << '\n';
			}

		}

		// LESS/EQUAL
		if (OPCODE == 7 || OPCODE == 8) {
			INT64 S1 = MEM[PC+1]; // SOURCE 1
			INT64 S2 = MEM[PC+2]; // SOURCE 2
			INT64 D1 = MEM[PC+3]; // DEST
			ARGS.push_back(&S1);
			ARGS.push_back(&S2);
			ARGS.push_back(&D1);

			if (OPCODE == 7)
				DISPLAY_INSTRUCTION(PC, "LESS", MODES, ARGS);
			if (OPCODE == 8)
				DISPLAY_INSTRUCTION(PC, "EQL", MODES, ARGS);

			APPLY_MODES(MEM, ARGS, MODES, BASE, 2);

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
		}

		// BASE ADJUST
		if (OPCODE == 9) {
			INT64 S1 = MEM[PC+1]; // SOURCE
			ARGS.push_back(&S1);

			DISPLAY_INSTRUCTION(PC, "BASE", MODES, ARGS);

			APPLY_MODES(MEM, ARGS, MODES, BASE);

			BASE += S1;

			if (DEBUG)
				cout << "BASE += " << S1 << '\n';

			PC += 2;
		}

	}

	return 1;
}

int main() {
	ifstream in("input");

	map<INT64, INT64> mem;
	INT64 base = 0;
	INT64 pc = 0;

	// load memory
	string s;
	int i=0;
	while (getline(in, s, ',')) {
		stringstream conv(s);
		int n;
		conv >> n;
		mem[i] = n;
		++i;
	}

	constexpr int width = 200;
	constexpr int height = 200;

	// initialize hull
	int hull[width * height];
	for (int i = 0; i < width*height; i++)
		hull[i] = 0;

	// starting point
	int x = width/2;
	int y = height/2;
	int dir = 0; // 0 == UP, 1 == RIGHT, 2 == DOWN, 3 == LEFT

	int statusCode = 2;

	// set starting point to white
	hull[y * width + x] = 1;

	while (statusCode != 0) {
		cout << x << ", " << y << '\n';

		if (x < 0 || x >= width || y < 0 || y >= height) {
			cerr << "WENT OUT OF BOUNDS, INCREASE SIZE" << '\n';
			break;
		}

		string panel = to_string(hull[y * width + x]);
		istringstream iss(panel);
		ostringstream oss;

		cout << "INPUT: " << panel << '\n';

		statusCode = run(mem, pc, base, iss, oss);

		string out = oss.str();

		cout << "OUTPUT: " << out << '\n';

		int color = out[0] - '0';
		int turn = out[1] - '0';

		if (statusCode != 0)
			hull[y * width + x] = color;

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

		cout << '\n';
	}

	// make registration image
	ofstream out("reg.pbm");

	out << "P1" << '\n';
	out << width << " " << height << '\n';
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (hull[y * width + x] == 1)
				out << 0 << ' ';
			else
				out << 1 << ' ';
		}
		out << '\n';
	}

	return 0;
}
