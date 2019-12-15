#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

// INTCODE COMPUTER OPCODES
constexpr int HALT = 99;
constexpr int ADD = 01;
constexpr int MUL = 02;

// Part 2 Input
constexpr int target = 19690720;

// Load intcode program into computer memory
vector<int> loadProgram(const string &filename) {
	ifstream in(filename);

	string intcode;
	vector<int> mem;
	while (getline(in, intcode, ','))
		mem.push_back(stoi(intcode));

	return mem;
}

// INTCODE COMPUTER
int RUN(vector<int> &MEM) {
	for (size_t i=0; i < MEM.size(); i += 4) {
		int OPCODE = MEM[i];

		if (OPCODE == HALT)
			break;

		// Load Instruction Arguments
		int S1 = MEM[i+1]; // Source Address 1
		int S2 = MEM[i+2]; // Source Address 2
		int D1 = MEM[i+3]; // Destination Address
		S1 = MEM[S1]; // Source Value 1
		S2 = MEM[S2]; // Source Value 2

		// Perform Instruction
		if (OPCODE == ADD)
			MEM[D1] = S1 + S2;
		if (OPCODE == MUL)
			MEM[D1] = S1 * S2;
	}

	return MEM[0];
}

void part1(const string &filename) {
	vector<int> mem = loadProgram(filename);

	// Set 1202 Program Alarm State
	if (filename == "input") {
		mem[1] = 12;
		mem[2] = 02;
	}

	int exitcode = RUN(mem);

	cout << "Exit Code: " << exitcode << '\n';
}

void part2(const string &filename) {
	vector<int> initial_mem = loadProgram(filename);

	for (int noun=0; noun < 100; noun++) {
		for (int verb=0; verb < 100; verb++) {

			// make a copy of the program and modify arguments of first instruction
			vector<int> mem = initial_mem;
			mem[1] = noun;
			mem[2] = verb;

			int exitcode = RUN(mem);

			if (exitcode == target) {
				cout << "Noun: " << noun << '\n';
				cout << "Verb: " << verb << '\n';

				int ans = 100 * noun + verb;
				cout << "Answer: " << ans << '\n';
				return;
			}
		}
	}
}

int main() {
	part1("input");
	part2("input");

	return 0;
}