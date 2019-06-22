#pragma once
#include <vector>
#include <future>
#include "Die.h"
#include "Player.h"

#ifdef _MSC_VER
#include <windows.h>
#else
#include <cstdlib>
#include <cstring>
#endif

class Night;

class Game
{
public:
	Game();
	void Run();


private:
	Player player;
//	std::vector<Night> night;
	int turn_count;
	Die d6{ 1, 6 };
	Die d4{ 1, 4 };
	Die d8{ 1, 8 };
	Die d10{ 1, 10 };
	Die d20{ 1, 20 };
	void PrintOptions();
	void ClearScreen();
};