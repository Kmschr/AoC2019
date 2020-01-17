#include "intcode.h"
#include <iostream>
#include <sstream>

using namespace std;

constexpr int width = 20;
constexpr int height = 20;

int main() {
    INTCODE droid("input");

    char *map = new char[width * height];

    int x = width/2;
    int y = height/2;

    while (true) {
        string input;
        cin >> input;

        int newX = x;
        int newY = y;

        istringstream iss(input);
        ostringstream oss("");
        droid.RUN(iss, oss);

        cout << oss.str() << '\n';

    }

    delete [] map;

    return 0;
}