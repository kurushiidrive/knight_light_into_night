#include "Game.h"
#include <future>
#include <iostream>
#include <chrono>

// helper function for the Game class's Run function
void Launch(std::future<void>);

// function for typing in the style we want
std::ostream& Type(std::string);

Game::Game() :
	turn_count{ 0 },
	player{}
{}

#ifdef _MSC_VER  //  Microsoft Visual C++

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

void Game::Run()
{
	/* LAUNCH SCREEN */

	// This is currently not running as the implementation to skip the prologue letter's displaying doesn't work
	// In order to test it, comment/uncomment the line that says std::cin.ignore()
	// UPDATE (22 June 2019 14:57) the below code now works.
	// I had to make the concurrent Launch function a nonmember function so that std::thread's library actions could work properly
	// while also keeping Game's current abstraction.
	// also the for-loop in Launch was unnecessary.

	// use an std::promise exit signal to display the story message while waiting for the player to press start.
	std::promise<void> exit_signal;
	std::future<void> future_obj = exit_signal.get_future();

	// start the thread that will display the message
	std::thread launching{ Launch, std::move(future_obj) };
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
	std::cin.ignore();

	ClearScreen();

	std::this_thread::sleep_for(std::chrono::seconds{ 2 });

	/* STARTING THE GAME */
	Type("This is the Night, which only the fiercest of Knights may challenge.\n");
	std::this_thread::sleep_for(std::chrono::milliseconds{ 1500 });
	Type("Arbane has chosen you, Sir ... [enter your character\'s name]\n\t");
	std::cout << ">   ";
	std::string tmp_name;
	std::getline(std::cin, tmp_name); player.Set_Name(tmp_name);
	Type("Right, Sir " + player.Get_Name() + ", the ... [enter the number corresponding to your profession]\n");
	Type("\t0:\tWarrior\n\t1:\tRanger\n\t2:\tWizard\n\t");
	std::cout << "$   ";
	int tmp_p;
	std::cin >> tmp_p;
	while (tmp_p != 0 && tmp_p != 1 && tmp_p != 2)
	{
		Type("I\'m sorry, but the program could not recognise your input.");
		std::this_thread::sleep_for(std::chrono::seconds{ 2 });
		Type(" Please retry.\n");
		std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });

		Type("\t0:\tWarrior\n\t1:\tRanger\n\t2:\tWizard\n\t");
		std::cout << "$   ";
		std::cin >> tmp_p;
	}
	player.Reset(tmp_p);
	Type("Indeed, you are Sir " + player.Get_Name() + ", the ");
	switch (player.Get_Prof())
	{
	case 0:
		Type("Warrior of Arbre.");
		break;
	case 1:
		Type("Ranger of Arbre.");
		break;
	case 2:
		Type("Wizard of Arbre.");
		break;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds{ 1300 });
	Type("\nYou must defeat the Night lest they swallow our sacred land.");
	std::this_thread::sleep_for(std::chrono::milliseconds{ 1500 });
	Type(" I know you are capable, and I wish you well.\n");
	std::this_thread::sleep_for(std::chrono::seconds{ 3 });

	ClearScreen();

	std::this_thread::sleep_for(std::chrono::seconds{ 1 });

	/* MAIN GAME */
	GameLoop();
}

void Game::PrintOptions()
{
	std::cout << "What would you like to do (" << player.Get_Action_Points() << " AP remaining):" << std::endl;
	std::cout << "\t0:\tconcede" << std::endl;
	std::cout << "\t1:\tNormal Ability (-1 AP)" << std::endl;
	std::cout << "\t2:\tAccel Guard (-1 AP)" << std::endl;
	std::cout << "\t3:\tAccel Strike (-1 AP)" << std::endl;
	std::cout << "\t4:\tgame stats" << std::endl;
	if (player.Get_Prof() == 1)
		std::cout << "\t5:\tSwitch Weapons (-0 AP, requires minimum 1 AP remaining)" << std::endl;
}

void Game::GameLoop()
{
	bool conceded = false, gameover = false;

	Night encounter;
	night.push(encounter);
	int turn_flag = die.Reset(0, 1)();
	turn_count++;

	while (!night.empty() && !conceded)
	{
		gameover = false; conceded = false;
		if (turn_count == 1)
		{
			Type("You have entered, and before you stands an enemy, of the Night.\n");
			std::this_thread::sleep_for(std::chrono::seconds{ 2 });
		}
		while (!gameover)
		{
			if (turn_flag)
			{
				// just in case
				std::vector<int> attack_data;
				std::vector<int>::iterator it;
				int armour_class;

				Type("It is Sir " + player.Get_Name() + "\'s turn (your turn).\n");
				std::this_thread::sleep_for(std::chrono::seconds{ 1 });
				PrintOptions();
				if (turn_count == 1 || turn_count == 2)
					Type("(It is recommended that on your first turn you check the game stats.)\n");
				std::cout << "$   ";
				int option;
				std::cin >> option;
				std::cout << "Response: " << option << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds{ 1 });

				switch (option)
				{
				case 0:
					Type("Sir " + player.Get_Name() + " has conceded.\n");
					std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });
					conceded = true;
					gameover = true;
					break;
				case 1:
					if (player.Get_Action_Points() <= 0)
					{
						Type("Not enough action points.\n");
						std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });
						break;
					}
					Type("Sir " + player.Get_Name() + " attacks the enemy NIGHT!\n");
					if ((int)player.Get_Prof() == 0)
						Type("\tSword Slash -\n");
					else if ((int)player.Get_Prof() == 1)
					{
						if (player.Get_Weapon().substr(0, player.Get_Weapon().find(' ')) == "bow")
							Type("\tSwift Shot -\n");
						else if (player.Get_Weapon().substr(0, player.Get_Weapon().find(' ')) == "daggers")
							Type("\tDeft Strike -\n");
					}
					else if ((int)player.Get_Prof() == 2)
						Type("\tEnergy Missile -\n");

					attack_data = player.Use_Normal_Ability(); it = attack_data.begin();
					armour_class = night.front().Get_DEF() + 10;
					Type("Your attack roll: " + std::to_string(attack_data[0] - player.Get_ATK() - player.Get_Atk_Bonus()) + "(+" + std::to_string(player.Get_ATK() + player.Get_Atk_Bonus()) + ")\tAC: " + std::to_string(armour_class) + "\n");
					std::this_thread::sleep_for(std::chrono::seconds{ 2 });

					// did the attack hit?
					if (attack_data[0] - player.Get_ATK() - player.Get_Atk_Bonus() != 1 && (attack_data[1] || attack_data[0] >= armour_class))
					{
						if (attack_data[4])
							Type("Accel Strike! Sir " + player.Get_Name() + " deals bonus damage!\n");
						if (attack_data[1])
						{
							Type("\tCritical Hit!! ");
							*(it + 2) *= 2;
						}
						if (attack_data[3])
						{
							std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });
							Type("FATAL!!!");
							*(it + 2) *= 5;
						}
						if (!attack_data[1] && !attack_data[3])
							Type("\tHit!!");
						std::this_thread::sleep_for(std::chrono::milliseconds{ 1500 });

						Type("\nDamage roll: " + std::to_string(attack_data[2] - player.Get_POW() - player.Get_Pow_Bonus()) + "(+" + std::to_string(player.Get_POW() + player.Get_Pow_Bonus()) + ") = " + std::to_string(attack_data[2]) + " damage\n");
						std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });
						Type("\tResist " + std::to_string(night.front().Get_RES()) + " --> " + std::to_string(attack_data[2] - night.front().Get_RES()) + " damage dealt.\n");

						night.front().Set_HP(night.front().Get_HP() - (attack_data[2] - night.front().Get_RES()));
						if (attack_data[2] - night.front().Get_RES() < 0)
							Type("The enemy NIGHT has just absorbed damage since their resistance was higher than the damage dealt!\n");
						std::this_thread::sleep_for(std::chrono::milliseconds{ 800 });
						Type("The enemy NIGHT now has " + std::to_string(night.front().Get_HP()) + " HP remaining.\n");
						std::this_thread::sleep_for(std::chrono::seconds{ 1 });

						if (attack_data[4] && !((int)player.Get_Prof() == 1 && player.Get_Weapon().substr(0, player.Get_Weapon().find(' ')) == "bow"))
						{
							Type("Severe recoil suffered by Sir " + player.Get_Name() + " due to Accel Strike: " + std::to_string(attack_data[5]) + " damage\n");
							std::this_thread::sleep_for(std::chrono::milliseconds{ 800 });
							Type("Sir " + player.Get_Name() + " now has " + std::to_string(player.Get_HP()) + " HP remaining.\n");
						}
						//int toughness = (int)ceil(0.3 * K_DEF_ORIG);                    // each time the player hits the KNIGHT, the KNIGHT gets harder to hit, self-buffing his DEF value.
						//k_DEF += toughness;
					}
					else
					{
						Type("\tMiss.\n");
						std::this_thread::sleep_for(std::chrono::milliseconds{ 800 });
						Type("The enemy NIGHT has " + std::to_string(night.front().Get_HP()) + " HP remaining.\n");
						//k_DEF = K_DEF_ORIG;
					}

					if (night.front().Get_HP() <= 0)
					{
						Type("Sir " + player.Get_Name() + " victory! You have cleared the Night before you.\t(in " + std::to_string(turn_count) + " turns.)\n");
						std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });
						Type("\n\nWould you like to continue? [y or n]   >   ");
						char yorn; std::cin >> yorn;
						while (yorn != 'y' && yorn != 'Y' && yorn != 'n' && yorn != 'N')
						{
							Type("I\'m sorry, but the program could not recognise your input.");
							std::this_thread::sleep_for(std::chrono::seconds{ 2 });
							Type(" Please retry.\n");
							std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });

							Type("\n\nWould you like to continue? [y or n]   >   ");
							std::cin >> yorn;
						}

						if (yorn == 'n' || yorn == 'N')
						{
							if (!night.empty())
								night.pop();
							Type("\nGame ");
							std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });
							Type("Over.\n");
							std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });
						}
						else if (yorn == 'y' || yorn == 'Y')
						{
							Type("\nYou advance further into the Night.\n\n");
							std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });
							night.front().Reset(night.front().Get_Level()+1);
							player.Reset_Action_Points();
						}

						gameover = true;
						turn_count = 1;
						break;
					}
					else if (player.Get_HP() <= 0) // later this should be another if statement and not an else-if
					{
						Type("Sir " + player.Get_Name() + " defeat! The Night consumed you, preventing your advance.\t(in " + std::to_string(turn_count) + "turns.)\n");
						std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });
						Type("\n\nWould you like to continue? [y or n]   >   ");
						char yorn; std::cin >> yorn;
						while (yorn != 'y' && yorn != 'Y' && yorn != 'n' && yorn != 'N')
						{
							Type("I\'m sorry, but the program could not recognise your input.");
							std::this_thread::sleep_for(std::chrono::seconds{ 2 });
							Type(" Please retry.\n");
							std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });

							Type("\n\nWould you like to continue? [y or n]   >   ");
							std::cin >> yorn;
						}

						if (yorn == 'n' || yorn == 'N')
						{
							if(!night.empty())
								night.pop();
							Type("\nGame ");
							std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });
							Type("Over.\n");
							std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });
						}
						else if (yorn == 'y' || yorn == 'Y')
						{
							Type("\nYou rise from the ground, determined to press on through the Night.\n\n");
							std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });
							player.Reset(player.Get_Prof());
							night.front().Reset_Action_Points();
						}
						
						gameover = true;
						turn_count = 1;
						break;
					}

					if (player.Get_Action_Points() <= 0)
					{
						turn_flag = false;
						turn_count++;
						player.Reset_Action_Points();
						std::this_thread::sleep_for(std::chrono::milliseconds{ 1500 });
						Type("End of Sir " + player.Get_Name() + "\'s turn.\n");
					}

					std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });

					break;
				case 2:
					if (!player.Accel_Guard())
						Type("Not enough action points or Accel Guard was already activated.\n");
					else
					{
						Type("Sir " + player.Get_Name() + " has activated Accel Guard. Bonus Defense against next attack.\n");
						if (player.Get_Action_Points() <= 0)
						{
							turn_flag = false;
							turn_count++;
							player.Reset_Action_Points();
							std::this_thread::sleep_for(std::chrono::milliseconds{ 1500 });
							Type("End of Sir " + player.Get_Name() + "\'s turn.\n");
						}
					}

					std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });

					break;
				case 3:
					if (!player.Accel_Strike())
						Type("Not enough action points or Accel Strike was already activated.\n");
					else
					{
						Type("Sir " + player.Get_Name() + " has activated Accel Strike. Bonus Damage dealt on next attack.\n");
						if (player.Get_Action_Points() <= 0)
						{
							turn_flag = false;
							turn_count++;
							player.Reset_Action_Points();
							std::this_thread::sleep_for(std::chrono::milliseconds{ 1500 });
							Type("End of Sir " + player.Get_Name() + "\'s turn.\n");
						}
					}

					std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });

					break;
				case 4:
					Type("Stats:\n");
					Type("  Sir " + player.Get_Name() + " Level " + std::to_string(player.Get_Level()) + "\n");
					Type("\tWeapon:\t" + player.Get_Weapon() + "\n");
					Type("\tHP:\t" + std::to_string(player.Get_HP()) + "\n");
					Type("\tATK:\t" + std::to_string(player.Get_ATK()) + "\n");
					Type("\tDEF:\t" + std::to_string(player.Get_DEF()) + "\n");
					Type("\tPOW:\t" + std::to_string(player.Get_POW()) + "\n");
					Type("\tRES:\t" + std::to_string(player.Get_RES()) + "\n");
					Type("  Sir " + player.Get_Name() + "\'s " + player.Get_Weapon() + " Stats:\n");
					Type("\tAttack Bonus:\t" + std::to_string(player.Get_Atk_Bonus()) + "\n");
					Type("\tDefense Bonus:\t" + std::to_string(player.Get_Def_Bonus()) + "\n");
					Type("\tPower Bonus:\t" + std::to_string(player.Get_Pow_Bonus()) + "\n");

					Type("Number of turns elapsed:\t" + std::to_string(turn_count - 1) + "\n");

					std::this_thread::sleep_for(std::chrono::seconds{ 5 });

					break;
				case 5:
					if (player.Get_Prof() != 1)
					{
						Type("I\'m sorry, but the program could not recognise your input.");
						std::this_thread::sleep_for(std::chrono::seconds{ 2 });
						Type(" Please reenter a number corresponding to the action you would like to take this turn.\n");
						std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });
						break;
					}
					
					if (player.Switch())
						Type("Sir " + player.Get_Name() + " has switched weapons. [check game stats for new weapon statistics]\n");
					else
						Type("Not enough action points.\n");

					if (player.Get_Action_Points() <= 0)
					{
						turn_flag = false;
						turn_count++;
						player.Reset_Action_Points();
						std::this_thread::sleep_for(std::chrono::milliseconds{ 1500 });
						Type("End of Sir " + player.Get_Name() + "\'s turn.\n");
					}

					std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });

					break;
				default:
					Type("I\'m sorry, but the program could not recognise your input.");
					std::this_thread::sleep_for(std::chrono::seconds{ 2 });
					Type(" Please reenter a number corresponding to the action you would like to take this turn.\n");
					std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });
				}

				std::cout << std::endl << std::endl << std::endl;
			}

			else
			{
				// just in case
				std::vector<int> attack_data;
				std::vector<int>::iterator it;
				int armour_class;

				Type("It is the enemy NIGHT\'s turn.\n");
				std::this_thread::sleep_for(std::chrono::seconds{ 1 });
				Type("The enemy NIGHT attacks Sir " + player.Get_Name() + "!\n");
				Type("\tNight Raze -\n");

				attack_data = night.front().Use_Normal_Ability(); it = attack_data.begin();
				armour_class = player.Get_DEF() + player.Get_Def_Bonus() + 10;
				Type("The enemy NIGHT\'s attack roll: " + std::to_string(attack_data[0] - night.front().Get_ATK()) + "(+" + std::to_string(night.front().Get_ATK()) + ")\tAC: " + std::to_string(armour_class) + "\n");
				std::this_thread::sleep_for(std::chrono::seconds{ 2 });

				// did it hit?
				if (attack_data[0] - night.front().Get_ATK() != 1 && (attack_data[1] || attack_data[0] >= armour_class))
				{
					if (player.Get_Accel_Guard())
					{
						// accel guard is applied before critical damage adjustment
						if (attack_data[1])
							* (it + 2) /= 3;
						*(it + 2) -= player.Get_RES() + player.Get_Level();
						if (attack_data[1])
							* (it + 2) *= 3;

						Type("Accel Guard! Sir " + player.Get_Name() + " gets bonus resistance!\n");
						player.Set_Accel_Guard(false);
						std::this_thread::sleep_for(std::chrono::milliseconds{ 700 });
					}
					if (attack_data[1])
						Type("\tCritical Hit!!");
					else
						Type("\tHit!!");
					std::this_thread::sleep_for(std::chrono::milliseconds{ 1500 });

					Type("\nDamage roll: " + std::to_string(attack_data[2] - night.front().Get_POW()) + "(+" + std::to_string(night.front().Get_POW()) + ") = " + std::to_string(attack_data[2]) + " damage\n");
					std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });
					Type("\tResist " + std::to_string(player.Get_RES()) + " --> " + std::to_string(attack_data[2] - player.Get_RES()) + " damage dealt.\n");

					player.Set_HP(player.Get_HP() - (attack_data[2] - player.Get_RES()));
					if (attack_data[2] - player.Get_RES() < 0)
						Type("Sir " + player.Get_Name() + " has just absorbed damage since their resistance was higher than the damage dealt!\n");
					std::this_thread::sleep_for(std::chrono::milliseconds{ 800 });
					Type("Sir " + player.Get_Name() + " now has " + std::to_string(player.Get_HP()) + " HP remaining.\n");
					std::this_thread::sleep_for(std::chrono::seconds{ 1 });
				}
				else
				{
					Type("\tMiss.\n");
					std::this_thread::sleep_for(std::chrono::milliseconds{ 800 });
					Type("Sir " + player.Get_Name() + " has " + std::to_string(player.Get_HP()) + " HP remaining.\n");
				}

				// death conditions
				if (night.front().Get_HP() <= 0)
				{
					Type("Sir " + player.Get_Name() + " victory! You have cleared the Night before you.\t(in " + std::to_string(turn_count) + " turns.)\n");
					std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });
					Type("\n\nWould you like to continue? [y or n]   >   ");
					char yorn; std::cin >> yorn;
					while (yorn != 'y' && yorn != 'Y' && yorn != 'n' && yorn != 'N')
					{
						Type("I\'m sorry, but the program could not recognise your input.");
						std::this_thread::sleep_for(std::chrono::seconds{ 2 });
						Type(" Please retry.\n");
						std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });

						Type("\n\nWould you like to continue? [y or n]   >   ");
						std::cin >> yorn;
					}

					if (yorn == 'n' || yorn == 'N')
					{
						if (!night.empty())
							night.pop();
						Type("\nGame ");
						std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });
						Type("Over.\n");
						std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });
					}
					else if (yorn == 'y' || yorn == 'Y')
					{
						Type("\nYou advance further into the Night.\n\n");
						std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });
						night.front().Reset(night.front().Get_Level()+1);
						player.Reset_Action_Points();
					}

					gameover = true;
					turn_count = 1;
					break;
				}
				else if (player.Get_HP() <= 0) // later this should be another if statement and not an else-if
				{
					Type("Sir " + player.Get_Name() + " defeat! The Night consumed you, preventing your advance.\t(in " + std::to_string(turn_count) + " turns.)\n");
					std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });
					Type("\n\nWould you like to continue? [y or n]   >   ");
					char yorn; std::cin >> yorn;
					while (yorn != 'y' && yorn != 'Y' && yorn != 'n' && yorn != 'N')
					{
						Type("I\'m sorry, but the program could not recognise your input.");
						std::this_thread::sleep_for(std::chrono::seconds{ 2 });
						Type(" Please retry.\n");
						std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });

						Type("\n\nWould you like to continue? [y or n]   >   ");
						std::cin >> yorn;
					}

					if (yorn == 'n' || yorn == 'N')
					{
						if (!night.empty())
							night.pop();
						Type("\nGame ");
						std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });
						Type("Over.\n");
						std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });
					}
					else if (yorn == 'y' || yorn == 'Y')
					{
						Type("\nYou rise from the ground, determined to press on through the Night.\n\n");
						std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });
						player.Reset(player.Get_Prof());
						night.front().Reset_Action_Points();
					}

					gameover = true;
					turn_count = 1;
					break;
				}

				if (night.front().Get_Action_Points() <= 0)
				{
					turn_flag = true;
					turn_count++;
					night.front().Reset_Action_Points();
					std::this_thread::sleep_for(std::chrono::milliseconds{ 1500 });
					Type("End of enemy NIGHT\'s turn.\n");
				}

				std::this_thread::sleep_for(std::chrono::milliseconds{ 2500 });
				std::cout << std::endl << std::endl << std::endl;
			}
		}
	}
	Type("\n\n[Now exiting...]\n");
}

void Launch(std::future<void> future_obj)
{
	std::string launch_message{ "The trees that conceived yesternight our Dear Fire from which*comes our Light (by which all in our land move, live, and*have breath--our most important resource) have abruptly been*no more. Unknown is the cause of this dread our land Arbre*now beholds. Some rumours say that our enemy, the Night, is at wake;*others still, say the world has met its bitter ends. Yet, even *amidst this vast darkness that extends from the edge of our land over*yonder and beyond, our hearts tell us that there is still a hope;*and though deeply distressed am I to wake each dawn to the surrounding *infinite darkness, devoid of our Dear Fire, oddly the Light shining*watching us from yonder stars, it I can still see. Much to carry is *the burden, and fearful is the thought that our people may not*embrace the warmth and beauty of our Dear Fire ever again. The suddenness*with which our foes have struck, doubtful it is that there is any*other cause of our being shaken. Even my dearest wife Arlayna, who comes*fond of knowledge and can willfully invoke it at her fingertips, knows*not of our enemy's whereabouts, or even who our enemy is. Thus, even*as outnumbered as be we might, I, Arbane IX of Leirmat, am left*to but one option as the Vanguard of Arbre: send our three*greatest Knights to oppose the Night face-to-face to the end*in order to restore the great Light which from us was robbed. Today, I send:*Knight Light into Night.***- Vanguard Arbane IX and his wife, Vanguardress Arlayna II." };

	std::chrono::milliseconds typing_delay{ 40 }, period_delay{ 2000 }, comma_delay{ 500 }, semicolon_delay{ 1000 }, colon_delay{ 250 };

	std::cout << "[Press START (enter) to skip.]" << std::endl << std::endl << std::endl;

	int i = 0;

	while (future_obj.wait_for(std::chrono::milliseconds{ 1 }) == std::future_status::timeout && i != launch_message.length())
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

		i++;
	}
}

std::ostream& Type(std::string str)
{
	for (int i = 0; i != str.length()-1; i++)
	{
		std::cout << str[i];
		std::this_thread::sleep_for(std::chrono::milliseconds{ 25 });
	}

	return std::cout << str[str.length() - 1];
}