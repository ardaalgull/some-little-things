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
	map += L"#........##....#";
	map += L"#####..........#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#.......########";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#....#######...#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
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

		//   Controls
		
		// Move Right
		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			fPlayerAngle -= (1.5f) * fElapsedTime;

		// Move Left
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			fPlayerAngle += (1.5f) * fElapsedTime;

		// Move Forward
		if (GetAsyncKeyState((unsigned short)'W') & 0x8000) 
		{
			fPlayerX += sinf(fPlayerAngle) * 5.0f * fElapsedTime;
			fPlayerY += cosf(fPlayerAngle) * 5.0f * fElapsedTime;
		}

		// Move Back
		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			fPlayerX -= sinf(fPlayerAngle) * 5.0f * fElapsedTime;
			fPlayerY -= cosf(fPlayerAngle) * 5.0f * fElapsedTime;
		}




		for (int x = 0; x < nScreenWidth; x++)
		{
			float fRayAngle = (fPlayerAngle - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

			float fDistance2Wall = 0;
			bool bHitWall = false;

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
					}
				}
			}
			
			int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistance2Wall);
			int nFloor = nScreenHeight - nCeiling;

			short nShade = ' ';

			// Wall shading types
			if (fDistance2Wall <= fDepth / 4.0f)                   nShade = 0x2588;  // very close
			else if (fDistance2Wall < fDepth / 3.0f)               nShade = 0x2593;
			else if (fDistance2Wall < fDepth / 2.0f)               nShade = 0x2592;
			else if (fDistance2Wall < fDepth)                      nShade = 0x2591;
			else                                                   nShade = ' ';     // too far away



			for (int y = 0; y < nScreenHeight; y++) 
			{
				if (y < nCeiling)
					screen[y * nScreenWidth + x] = ' ';
				else if (y > nCeiling && y <= nFloor)
					screen[y * nScreenWidth + x] = nShade;
				else 
				{
					float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
					if (b < 0.25) nShade = '#';
					else if (b < 0.5) nShade = 'X';
					else if (b < 0.75) nShade = '.';
					else if (b < 0.9) nShade = '-';
					else nShade = ' ';

					screen[y * nScreenWidth + x] = nShade;
				}
				    

			}

		}








		screen[nScreenWidth * nScreenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}



	return 0;
}
