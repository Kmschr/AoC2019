#include <iostream>
using namespace std;

bool passIsValid(int pass) {
	bool doub = false;
	string s = to_string(pass);	

	for (int i=0; i < 5; ++i) {
		if (s[i] == s[i+1])
			doub = true;
		if (s[i] > s[i+1])
			return false;
	}

	return doub;
}

int main() {
	int low = 353096;
	int high = 843212;
	int n = 0;

	for (int i=low; i <= high; ++i)
		if (passIsValid(i))
			++n;

	cout << n << '\n';

	return 0;
}
