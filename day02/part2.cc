#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

int run(vector<int> &mem) {
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
	
	return mem[0];
}

int main() {
	ifstream in("input");

	vector<int> initial_mem;

	string s;
	while (getline(in, s, ',')) {
		stringstream conv(s);
		int n;
		conv >> n;
		initial_mem.push_back(n);
	}

	for (int noun=0; noun < 100; ++noun) {
		for (int verb=0; verb < 100; ++verb) {
			vector<int> mem = initial_mem;
			mem[1] = noun;
			mem[2] = verb;

			int result = run(mem);
			if (result == 19690720) {
				int ans = 100 * noun + verb;
				cout << ans << '\n';
				exit(0);
			}
		}
	}

	return 0;
}