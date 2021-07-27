#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <chrono>

using namespace std;

int nScreenWidth = 120;
int nScreenHeight = 40;
int nMapHeight = 16;
int nMapWidth = 16;

float fPlayerX = 8.0f;
float fPlayerY = 8.0f;
float fPlayerAngle = 0.0f;
float fFOV = 3.14159f / 4.0f;
float fDepth = 16.0f;
float fSpeed = 5.0f;



int main()
{
	// Screen Buffer
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	wstring map;
	map += L"################";
	map += L"#..............#";
	map += L"##.............#";
	map += L"#..............#";
	map += L"#..........#####";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#####..........#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#............###";
	map += L"#..............#";
	map += L"################";

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();


	while (1) // Game loop here
	{
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();

		// ===== Controls =====

		// Move Left
		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			fPlayerAngle -= (1.5f) * fElapsedTime;

		// Move Right
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			fPlayerAngle += (1.5f) * fElapsedTime;

		// Move Forward
		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		{
			fPlayerX += sinf(fPlayerAngle) * fSpeed * fElapsedTime;;
			fPlayerY += cosf(fPlayerAngle) * fSpeed * fElapsedTime;;
			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			{
				fPlayerX -= sinf(fPlayerAngle) * fSpeed * fElapsedTime;;
				fPlayerY -= cosf(fPlayerAngle) * fSpeed * fElapsedTime;;
			}
		}

		// Move Back
		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			fPlayerX -= sinf(fPlayerAngle) * fSpeed * fElapsedTime;;
			fPlayerY -= cosf(fPlayerAngle) * fSpeed * fElapsedTime;;
			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			{
				fPlayerX += sinf(fPlayerAngle) * fSpeed * fElapsedTime;;
				fPlayerY += cosf(fPlayerAngle) * fSpeed * fElapsedTime;;
			}
		}




		for (int x = 0; x < nScreenWidth; x++)
		{
			float fRayAngle = (fPlayerAngle - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

			float fDistance2Wall = 0;
			bool bHitWall = false;
			bool bBoundary = false;

			float fEyeX = sinf(fRayAngle);
			float fEyeY = cosf(fRayAngle);

			while (!bHitWall && fDistance2Wall < fDepth)
			{


				fDistance2Wall += 0.1f;

				int nTestX = (int)(fPlayerX + fEyeX * fDistance2Wall);
				int nTestY = (int)(fPlayerY + fEyeY * fDistance2Wall);

				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
				{
					bHitWall = true;
					fDistance2Wall = fDepth;
				}
				else
				{
					if (map[nTestY * nMapWidth + nTestX] == '#')
					{
						bHitWall = true;

						vector<pair<float, float>> p;

						for (int tx = 0; tx < 2; tx++)
							for (int ty = 0; ty < 2; ty++)
							{
								float vy = (float)nTestY + ty - fPlayerY;
								float vx = (float)nTestX + tx - fPlayerX;
								float d = sqrt(vx * vx + vy * vy);
								float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
								p.push_back(make_pair(d, dot));
							}

						sort(p.begin(), p.end(), [](const pair<float, float>& left, const pair<float, float>& right) {return left.first < right.first; });	

						float fBound = 0.01;
						if (acos(p.at(0).second) < fBound) bBoundary = true;
						if (acos(p.at(1).second) < fBound) bBoundary = true;
						if (acos(p.at(2).second) < fBound) bBoundary = true;
					}
				}
			}

			int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistance2Wall);
			int nFloor = nScreenHeight - nCeiling;

			short nShade = ' ';

			// Wall shading types
			if (fDistance2Wall <= fDepth / 4.0f)                   nShade = 0x2588;  
			else if (fDistance2Wall < fDepth / 3.0f)               nShade = 0x2593;
			else if (fDistance2Wall < fDepth / 2.0f)               nShade = 0x2592;
			else if (fDistance2Wall < fDepth)                      nShade = 0x2591;
			else                                                   nShade = ' ';     

			if (bBoundary) nShade = ' ';

			for (int y = 0; y < nScreenHeight; y++)
			{
				if (y <= nCeiling)
					screen[y * nScreenWidth + x] = ' ';
				else if (y > nCeiling && y <= nFloor)
					screen[y * nScreenWidth + x] = nShade;
				else
				{
					// Floor shading
					float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
					if (b < 0.25)		nShade = '#';
					else if (b < 0.5)	nShade = 'x';
					else if (b < 0.75)	nShade = '.';
					else if (b < 0.9)	nShade = '-';
					else				nShade = ' ';
					screen[y * nScreenWidth + x] = nShade;
				}
			}

		}



		// Stats
		swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", fPlayerX, fPlayerY, fPlayerAngle, 1.0f / fElapsedTime);
		 
		// Map
		for (int nx = 0; nx < nMapWidth; nx++)
			for (int ny = 0; ny < nMapWidth; ny++) {
				screen[(ny + 1) * nScreenWidth + nx] =map[ny * nMapWidth + nx];
			}

		screen[((int)fPlayerY + 1) * nScreenWidth + (int)fPlayerX] = 'P';

		screen[nScreenWidth * nScreenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}



	return 0;
}