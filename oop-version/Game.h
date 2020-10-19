#pragma once
#include <queue>
#include <future>
#include "Die.h"
#include "Player.h"
#include "Night.h"

#ifdef _MSC_VER
#include <windows.h>
#else
#include <cstdlib>
#include <cstring>
#endif

class Game
{
public:
	Game();
	void Run();


private:
	Player player;
	std::queue<Night> night;
	int turn_count;
	Die die;
	void PrintOptions();
	void ClearScreen();
	void GameLoop();
};