/*
BSD 3-Clause License
Copyright (c) 2023, Алекс
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <chrono>
#include <list>

#include <Windows.h>

using namespace std;

constexpr int nScreenWidth = 120;
constexpr int nScreenHeight = 30;

int main()
{
	wchar_t screen[nScreenWidth * nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);

	SetConsoleTitle(L"Snake");

	int nRecord = 0;

	while (1)
	{
		int nAppleX = nScreenWidth / 4;
		int nAppleY = nScreenHeight / 2;

		int nDir = 0;
		int nScore = 0;

		bool bDead = false;

		list<pair<int, int>> listSnake = { {60,15},{61,15},{62,15},{63,15} };

		srand(time(NULL));

		while (!bDead)
		{
			// Make a delay

			auto t1 = chrono::system_clock::now();
			while ((chrono::system_clock::now() - t1) < ((nDir % 2 == 0) ? 120ms : 160ms))
			{
				// Get user input

				if (GetAsyncKeyState(L'A') & 0x8000)
					nDir = 0;

				if (GetAsyncKeyState(L'D') & 0x8000)
					nDir = 2;

				if (GetAsyncKeyState(L'W') & 0x8000)
					nDir = 1;

				if (GetAsyncKeyState(L'S') & 0x8000)
					nDir = 3;
			}

			// Update snake position

			switch (nDir)
			{
			case 0: listSnake.push_front({ listSnake.front().first - 1, listSnake.front().second }); break;
			case 1: listSnake.push_front({ listSnake.front().first, listSnake.front().second - 1 }); break;
			case 2: listSnake.push_front({ listSnake.front().first + 1, listSnake.front().second }); break;
			case 3: listSnake.push_front({ listSnake.front().first, listSnake.front().second + 1 }); break;
			}

			// Check for collision with apple
			if (listSnake.front().first == nAppleX && listSnake.front().second == nAppleY)
			{
				nScore++;

				nAppleX = rand() % nScreenWidth;
				nAppleY = (rand() % (nScreenHeight - 3)) + 3;

				for (int i = 0; i < 5; i++)
					listSnake.push_back({ listSnake.back().first, listSnake.back().second });
			}

			// Check for collision with body

			for (list<pair<int, int>>::iterator i = listSnake.begin(); i != listSnake.end(); i++)
			{
				if (i != listSnake.begin() && i->first == listSnake.front().first && i->second == listSnake.front().second)
					bDead = true;
			}

			// Check for collision with world

			if (listSnake.front().first < 0 || listSnake.front().first >= nScreenWidth)
				bDead = true;

			if (listSnake.front().second < 3 || listSnake.front().second >= nScreenHeight)
				bDead = true;

			// Cut snake tail
			listSnake.pop_back();

			// Update screen

			for (int i = 0; i < nScreenWidth * nScreenHeight; i++)
				screen[i] = '\0';

			// Display Score
			for (int i = 0; i < nScreenWidth; i++)
			{
				screen[i] = L'=';
				screen[2 * nScreenWidth + i] = L'=';
			}

			wsprintf(&screen[nScreenWidth + 5], L"SCORE: %d", nScore);
			wsprintf(&screen[nScreenWidth * 2 - 15], L"RECORD: %d", nRecord);

			// Draw snake body
			for (auto& s : listSnake)
				screen[s.second * nScreenWidth + s.first] = (bDead ? L'+' : L'#');

			// Draw snake head
			screen[listSnake.front().second * nScreenWidth + listSnake.front().first] = (bDead ? L'X' : L'@');

			// Draw apple
			screen[nAppleY * nScreenWidth + nAppleX] = L'O';

			// Check for death
			if (bDead)
				wsprintf(&screen[nScreenHeight / 2 * nScreenWidth + nScreenWidth / 3], L"GAME OVER! PRESS SPACE TO PLAY AGAIN!");

			DWORD dwBytesWritten = 0;
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwBytesWritten);
		}

		nRecord = nScore;

		// Wait until user press space
		while (!(GetAsyncKeyState(VK_SPACE) & 0x8000));
	}

	return 0;
}