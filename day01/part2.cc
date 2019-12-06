#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

int main() {
	ifstream in("input");

	if (!in)
		throw runtime_error("Can't open input file");

	long sum = 0;
	string line;
	while (getline(in, line)) {
		stringstream conv(line);

		int n;
		conv >> n;

		while (n > 0) {
			int k = n / 3 - 2;
			if (k > 0)
				sum += k;
			n = k;
		}
	}

	cout << sum << '\n';

	return 0;
}
