#include <chrono>
#include <list>
#include <string>

#include <Windows.h>

using namespace std;

constexpr int nScreenWidth = 120;
constexpr int nScreenHeight = 30;

int main()
{
	wchar_t screen[nScreenWidth * nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);

	SetConsoleTitle(L"Break Out in Command Prompt");

	while (1)
	{
		bool bDead = false;

		auto tp1 = chrono::system_clock::now();
		auto tp2 = chrono::system_clock::now();

		float fDeltaTime = 0.0f;

		float fBatPosX = float(nScreenWidth / 2);
		float fBatPosY = float(nScreenHeight - 3);
		float fBatVel = 30.0f;

		int nBatWidth = 5;

		float fBallPosX = nScreenWidth / 2;
		float fBallPosY = nScreenHeight / 2;

		float fBallDirX = cosf(-0.4f);
		float fBallDirY = sinf(-0.4f);

		float fBallSpeed = 15.0f;

		bool bSpacePressed = false;

		int nBlockOffsetX = 55;
		int nBlockOffsetY = 8;

		int nWallOffset = 5;

		constexpr int nBlocksWidth = 15;
		constexpr int nBlocksHeight = 4;

		int blocks[nBlocksWidth * nBlocksHeight];

		// Set health to each block
		for (int i = 0; i < nBlocksWidth * nBlocksHeight; i++)
			blocks[i] = 1;

		int nScore = 0;

		while (!bDead)
		{
			// Calculate delta time
			tp2 = chrono::system_clock::now();
			fDeltaTime = chrono::duration<float>(tp2 - tp1).count();
			tp1 = tp2;

			// Get user input
			if ((GetAsyncKeyState(VK_SPACE) & 0x8000))
			{
				if (!bSpacePressed)
				{
					fBatVel *= -1;
					bSpacePressed = true;
				}
			}
			else
				bSpacePressed = false;

			// Calculate potential ball position
			float fPotBallX = fBallPosX + fBallSpeed * fBallDirX * fDeltaTime;
			float fPotBallY = fBallPosY + fBallSpeed * fBallDirY * fDeltaTime;

			auto PointVsPoint = [&](float x, float y)
			{
				// Point to test collision with
				int nTestX = int(fPotBallX + x);
				int nTestY = int(fPotBallY + y);

				int nBlockX = nTestX - nBlockOffsetX;
				int nBlockY = nTestY - nBlockOffsetY;

				// if collision was detected
				if (nBlockX >= 0 && nBlockY >= 0 && nBlockX < nBlocksWidth && nBlockY < nBlocksHeight)
				{
					auto& block = blocks[nBlockY * nBlocksWidth + nBlockX];

					if (block == 0)
						return false;

					// decrease health of block
					block--;

					if (block == 0)
						nScore++;

					// change direction of ball
					if (x == 0.0f) fBallDirY *= -1.0f;
					if (y == 0.0f) fBallDirX *= -1.0f;

					return true;
				}
				else if (nTestX == nWallOffset || nTestY == nWallOffset || nTestX == nScreenWidth - nWallOffset)
				{
					// change direction of ball
					if (x == 0.0f) fBallDirY *= -1.0f;
					if (y == 0.0f) fBallDirX *= -1.0f;

					return true;
				}
				else if ((int)fBatPosX <= nTestX && nTestX <= (int)fBatPosX + nBatWidth && nTestY == (int)fBatPosY)
				{
					// change direction of the ball
					fBallDirY *= -1.0f;
					return true;
				}
				else
				{
					// No collision was detected
					return false;
				}
			};

			// Check for collision with radial tiles
			bool bHitTile = false;
			bHitTile |= PointVsPoint(+0.0f, -1.0f);
			bHitTile |= PointVsPoint(+0.0f, +1.0f);
			bHitTile |= PointVsPoint(-1.0f, +0.0f);
			bHitTile |= PointVsPoint(+1.0f, +0.0f);

			// Update ball pos
			fBallPosX += fBallSpeed * fBallDirX * fDeltaTime;
			fBallPosY += fBallSpeed * fBallDirY * fDeltaTime;

			// Check boundary
			if ((int)fBallPosX < nWallOffset || (int)fBallPosY < nWallOffset || (int)fBallPosX > nScreenWidth - nWallOffset || (int)fBallPosY >(int)fBatPosY)
				bDead = true;

			// Update bat pos
			fBatPosX += fBatVel * fDeltaTime;

			// Check boundary
			if (fBatPosX < 11.0f)									    fBatPosX = 11.0f;
			if (fBatPosX + (float)nBatWidth > float(nScreenWidth - 10)) fBatPosX = float(nScreenWidth - 10) - (float)nBatWidth;

			// Clear screen
			for (int i = 0; i < nScreenWidth * nScreenHeight; i++)
				screen[i] = '\0';

			// Draw boundary
			for (int x = nWallOffset; x < nScreenWidth - nWallOffset; x++)
				screen[nWallOffset * nScreenWidth + x] = L'=';

			for (int y = nWallOffset; y < nScreenHeight - nWallOffset; y++)
				screen[y * nScreenWidth + nWallOffset] = L'|';

			for (int y = nWallOffset; y < nScreenHeight - nWallOffset; y++)
				screen[y * nScreenWidth + (nScreenWidth - nWallOffset)] = L'|';

			// Draw bat
			for (int x = 0; x < nBatWidth; x++)
				screen[(int)fBatPosY * nScreenWidth + ((int)fBatPosX + x)] = L'#';

			// Draw blocks
			for (int x = 0; x < nBlocksWidth; x++)
				for (int y = 0; y < nBlocksHeight; y++)
				{
					int b = blocks[y * nBlocksWidth + x];
					if (b > 0)
						screen[(y + nBlockOffsetY) * nScreenWidth + (x + nBlockOffsetX)] = to_wstring(b).c_str()[0];
				}

			// Draw ball
			screen[(int)fBallPosY * nScreenWidth + (int)fBallPosX] = L'O';

			// Draw score
			wsprintf(&screen[nScreenWidth + 5], L"SCORE: %d", nScore);

			// Check for death
			if (bDead)
				wsprintf(&screen[nScreenHeight / 2 * nScreenWidth + nScreenWidth / 3], L"GAME OVER! PRESS SPACE TO PLAY AGAIN!");

			// Update console buffer
			DWORD dwBytesWritten = 0;
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwBytesWritten);
		}

		// Wait until user press space
		while (!(GetAsyncKeyState(VK_SPACE) & 0x8000));
	}
}
