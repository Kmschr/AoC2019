// WINDOWS ONLY
// COMPILE WITH -DUNICODE IN g++

#include "intcode.h"
#include <windows.h>
#include <thread>
#include <chrono>
#include <sstream>

using namespace std;

constexpr size_t width = 40;
constexpr size_t height = 25;

constexpr wchar_t ball = L'O';
constexpr wchar_t paddle = L'-';
constexpr wchar_t block = 0x2592;
constexpr wchar_t wall = 0x2588;

void updateScreen(wchar_t *screen, int &score, const ostringstream &oss) {
	string line;
	istringstream screenData(oss.str());
	while(getline(screenData, line)) {
		int x;
		int y;
		int tile;

		int i=0;
		string num;
		istringstream pixel(line);
		while (getline(pixel, num, ',')) {
			istringstream conv(num);
			if (i == 0)
				conv >> x;
			if (i == 1)
				conv >> y;
			if (i == 2)
				conv >> tile;

			i++;
		}

		if (x == -1 && y == 0) {
			score = tile;
			continue;
		}

		if (tile == 0)
			screen[y*width + x] = L' ';
		if (tile == 1)
			screen[y*width + x] = wall;
		if (tile == 2)
			screen[y*width + x] = block;
		if (tile == 3)
			screen[y*width + x] = paddle;
		if (tile == 4)
			screen[y*width + x] = ball;
	}
}

void displayScreen(wchar_t *screen) {
	for (auto y=0; y < height; y++) {
		for (auto x=0; x < width; x++) {
			char c = screen[y*width + x];
			if (c != '!')
				cout << c;
		}
		cout << '\n';
	}
}

void part1(INTCODE game) {
	ostringstream oss;

	game.RUN(cin, oss);
	wchar_t screen[width*height];
	for (auto i=0; i < width*height; i++)
		screen[i] = L'!';

	int score=0;
	updateScreen(screen, score, oss);

	int numBlock = 0;
	for (auto i=0; i < width*height; i++)
		if (screen[i] == L'B')
			numBlock++;

	cout << numBlock << '\n';
}

void part2(INTCODE game) {
	game.SET_MEM(0, 2);

	HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r); //stores the console's current dimensions

	auto screenWidth = r.right - r.left - 13;
	auto screenHeight = r.bottom - r.top;

	// Create Screen Buffer
	wchar_t *cmdScreen = new wchar_t[screenWidth*screenHeight+1];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	//SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	wchar_t screen[width * height];
	for (auto i=0; i < width*height; i++)
		screen[i] = L'!';

	cmdScreen[screenWidth * screenHeight] = '\0';

	int score = 0;
	int nextMove = 0;

	while (game.IS_RUNNING()) {
		for (auto i=0; i < screenWidth*screenHeight; i++)
			cmdScreen[i] = L' ';

		string dir = "0";
		if (nextMove == 1)
			dir = "1";
		if (nextMove == -1)
			dir = "-1";

		/* MANUAL CONTROL MODE
		Sleep(200)l
		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			dir = "-1";
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			dir = "1";  */

		istringstream iss(dir);
		ostringstream oss;
		game.RUN(iss, oss);

		updateScreen(screen, score, oss);

		int paddlePos;
		int ballPos;

		for (auto y=0; y < height; y++) {
			for (auto x=0; x < width; x++) {
				if (screen[y*width +x] == paddle)
					paddlePos = x;
				if (screen[y*width +x] == ball)
					ballPos = x;
				
				cmdScreen[y*screenWidth + x] = screen[y*width + x];
			}
		}

		if (ballPos == paddlePos)
			nextMove = 0;
		if (ballPos < paddlePos)
			nextMove = -1;
		if (ballPos > paddlePos)
			nextMove = 1;

		string sscore = "SCORE: " + to_string(score);
		for (auto i=0; i < sscore.size(); i++)
			screen[i+10] = sscore[i];
		//WriteConsoleOutputCharacter(hConsole, cmdScreen, screenWidth*screenHeight+1, { 0,0 }, &dwBytesWritten);
	}

	cout << "SCORE: " << score << '\n';
}

int main() {
	INTCODE game("input");

	//part1(game);
	part2(game);

	return 0;
}