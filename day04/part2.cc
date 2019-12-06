#include <iostream>
#include <assert.h>
using namespace std;

bool passIsValid(int pass) {
	bool doub = false;
	int skips = 0;
	string s = to_string(pass);	

	for (int i=0; i < 5; ++i) {
		if (skips == 0) {
			int k = i+1;
			int numMatching = 1;

			while (k < 6 && s[i] == s[k]) {
				++k;
				++numMatching;
			}

			if (numMatching > 1) {
				if (numMatching == 2)
					doub = true;
				
				skips = numMatching - 1;
			}
		} else {
			--skips;
		}

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

	assert(passIsValid(112233));
	assert(!passIsValid(123444));
	assert(passIsValid(111122));
	assert(!passIsValid(123456));

	return 0;
}
