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

		sum += n / 3 - 2;
		cout << n << '\n';
	}

	cout << sum << '\n';

	return 0;
}