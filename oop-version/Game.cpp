#include "Game.h"
#include <future>
#include <iostream>
#include <chrono>

Game::Game() :
	night{ 5 },
	turn_count{ 0 },
	player{}
{
	;
}

#ifdef _MSC_VER  //  Microsoft Visual C++

#include <windows.h>

void Game::ClearScreen()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	COORD upperLeft = { 0, 0 };
	DWORD dwCharsWritten;
	FillConsoleOutputCharacter(hConsole, TCHAR(' '), dwConSize, upperLeft, &dwCharsWritten);
	SetConsoleCursorPosition(hConsole, upperLeft);
}

#else  // not Microsoft Visual C++, so assume UNIX interface

#include <cstring>
#include <cstdlib>

void Game::ClearScreen()  // will just write a newline in an Xcode output window
{
	static const char* term = getenv("TERM");
	if (term == nullptr || strcmp(term, "dumb") == 0)
		std::cout << std::endl;
	else
	{
		static const char* ESC_SEQ = "\x1B[";  // ANSI Terminal esc seq:  ESC [
		std::cout << ESC_SEQ << "2J" << ESC_SEQ << "H" << std::flush;
	}
}

#endif

void Game::Launch(std::future<void> future_obj)
{
	std::string launch_message{ "The trees that conceived yesternight our Dear Fire from which*comes our Light (by which all in our land move, live, and*have breath--our most important resource) have abruptly been*no more. Unknown is the cause of this dread our land Arbre*now beholds. Some rumours say that our enemy, the Night, is at wake;*others still, say the world has met its bitter ends. Yet, even *amidst this vast darkness that extends from the edge of our land over*yonder and beyond, our hearts tell us that there is still a hope;*and though deeply distressed am I to wake each dawn to the surrounding *infinite darkness, devoid of our Dear Fire, oddly the Light shining*watching us from yonder stars, it I can still see. Much to carry is *the burden, and fearful is the thought that our people may not*embrace the warmth and beauty of our Dear Fire ever again. The suddenness*with which our foes have struck, doubtful it is that there is any*other cause of our being shaken. Even my dearest wife Arlayna, who comes*fond of knowledge and can willfully invoke it at her fingertips, knows*not of our enemy's whereabouts, or even who our enemy is. Thus, even*as outnumbered as be we might, I, Arbane IX of Leirmat, am left*to but one option as the Vanguard of Arbre: send our three*greatest Knights to oppose the Night face-to-face to the end*in order to restore the great Light which from us was robbed. Today, I send:*Knight Light into Night.***- Vanguard Arbane IX and his wife, Vanguardress Arlayna II." };

	std::chrono::milliseconds typing_delay{ 40 }, period_delay{ 2000 }, comma_delay{ 500 }, semicolon_delay{ 1000 }, colon_delay{ 250 };

	std::cout << "[Press START (enter) to skip.]" << std::endl << std::endl << std::endl;

	while (future_obj.wait_for(std::chrono::milliseconds{ 1 }) == std::future_status::timeout)
	{
		for (int i = 0; i != launch_message.length(); i++)
		{
			if (launch_message[i] == '*')
				std::cout << '\n';
			else
				std::cout << launch_message[i];

			if (launch_message[i] == '.')
				std::this_thread::sleep_for(period_delay);
			else if (launch_message[i] == ',')
				std::this_thread::sleep_for(comma_delay);
			else if (launch_message[i] == ';')
				std::this_thread::sleep_for(semicolon_delay);
			else if (launch_message[i] == ':')
				std::this_thread::sleep_for(colon_delay);
			else
				std::this_thread::sleep_for(typing_delay);
		}
	}
}

void Game::Run()
{
	/* LAUNCH SCREEN */

	// This is currently not running as the implementation to skip the prologue letter's displaying doesn't work
	// In order to test it, comment/uncomment the line that says std::cin.ignore()

	// use an std::promise exit signal to display the story message while waiting for the player to press start.
	std::promise<void> exit_signal;
	std::future<void> future_obj = exit_signal.get_future();

	// start the thread that will display the message
	std::thread launching{ &Game::Launch, std::move(future_obj) };
	std::cin.ignore();

	// once the player presses start, set the exit signal and call for the message display thread to join.
	exit_signal.set_value();
	launching.join();

	ClearScreen();

	std::this_thread::sleep_for(std::chrono::seconds{ 2 });

	/* TITLE SCREEN */
	std::string title{ "Knight Light into Night" };

	for (int i = 0; i != title.length(); i++)
	{
		std::cout << title[i];

		if (i == 6 || i == 12)
			std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });
		else
			std::this_thread::sleep_for(std::chrono::milliseconds{ 50 });
	}

	std::this_thread::sleep_for(std::chrono::seconds{ 2 });

	std::cout << std::endl << std::endl;
	std::cout << "[Press START (enter)]";
	std::cin.get();

	ClearScreen();

	std::this_thread::sleep_for(std::chrono::seconds{ 2 });

}

void Game::PrintOptions()
{
	std::cout << "What would you like to do:" << std::endl;
	std::cout << "\t0:\tconcede" << std::endl;
	std::cout << "\t1:\tattack" << std::endl;
	std::cout << "\t2:\tAccel Guard" << std::endl;
	std::cout << "\t3:\tAccel Strike" << std::endl;
	std::cout << "\t4:\tgame stats" << std::endl;
}