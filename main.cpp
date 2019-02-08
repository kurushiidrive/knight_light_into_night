#include <random>		// C++'11 only; so is nullptr
#include <iostream>		// cin and cout objects
#include <string>		// getline() and string
#include <cmath>		// ceil function
#include <ctime>		// time function
//#include <cstdlib>		// srand and rand functions

using namespace std;

//random_device rd; <- the random number previously used to seed the mersenne-twister
//mt19937 gen (rd()); <- the reason it was taken out was because it isn't a runtime random number
mt19937 gen (static_cast<mt19937::result_type>(time(nullptr)));
uniform_int_distribution<int> dist;		// default interval [0, INT_MAX]

static int roll (int sides, int times = 1)
{
	int sum = 0;
	dist.param (uniform_int_distribution<int>::param_type (1,sides));		// changes the interval of our random number distribution to [1,sides]
	for (int i = 0; i < times; i++)
		sum += dist(gen);
	return sum;
}

static void printOptions ()
{
	cout << "What would you like to do:" << endl;
	cout << "\t0:\tconcede" << endl;
	cout << "\t1:\tattack" << endl;
	cout << "\t2:\tAccel Guard" << endl;
	cout << "\t3:\tAccel Strike" << endl;
	cout << "\t4:\tgame stats" << endl;
//	cout << "\t5:\tBubble Shield" << endl;
}

/*
 * Godfried Boateng; last modified 17 June 2018
 * The earlier version of Knight Light into Night I had made while learning new C++11 features
 * spring of 2018. This can be considered the "short" version of the game (since it is only one battle).
 */

int main ()
{
//	srand (static_cast<unsigned int>(time(0)));		// outdated pseudorandom number generators from C
	int turn_count = 0;
	int ATK, DEF, POW, RES;		// attack, defense, power, and resistance
	int level;
	int HP;
	
	string weapon = "";
	int atk_bonus = 0, def_bonus = 0, pow_bonus = 0;
	
	cout << "Welcome to Knight Light into Night!" << endl;
	cout << "Please, enter your name: ";
	string player;
	getline (cin, player);
	
	cout << "Enter a level (any number from 1 to 1000): ";
	cin >> level;
	while (level <= 0 || level > 1000)
	{
		cout << "That was not within range! Please try again!\n" << endl;
		cout << "Enter a level (any number from 1 to 1000): ";
		cin >> level;
	}
	
	// Sir [player]'s Stats
	ATK = level + 10 + roll (8);
	DEF = level + 8 + roll (6);
	POW = level + 12 + roll (6);
	RES = level + 20 + roll (2);
	HP = (int) ceil (48.81 + (1.29*level)) + roll (12, level);
	
	cout << "Select a weapon among these three choices:\n";
	cout << "\t0:\tDaggers" << endl;
	cout << "\t1:\tBow" << endl;
	cout << "\t2:\tSword" << endl;
	int selection;
	cin >> selection;
	while (selection < 0 || selection > 2)
	{
		cout << "That was not among the selections available! Please try again!\n" << endl;
		cout << "Select a weapon among these three choices:\n";
		cout << "\t0:\tDaggers" << endl;
		cout << "\t1:\tBow" << endl;
		cout << "\t2:\tSword" << endl;
		cin >> selection;
	}
	
	switch (selection)
	{
		case 0:							// daggers; also able to crit and fatal more easily: crit on 18, 19, 20; fatal on 46, 47, 48, 49, and 50.
			ATK -= (int) ceil (1.2 * level);
			DEF += (int) ceil (2.1 * level);
			POW += (int) ceil (1.0 * level);
			
			weapon = "Daggers";
			atk_bonus = (int) ceil (1.2 * level)  * -1;
			def_bonus = (int) ceil (2.1 * level);
			pow_bonus = (int) ceil (1.0 * level);
			break;
		case 1:							// bow; easier to hit, but harder to defend (since a bow occupies 2H); fatals very easily (any number between 41 and 50, inclusive), and can Accel Strike without recoil since it's a ranged weapon.
			ATK += (int) ceil (2.4 * level);
			DEF -= (int) ceil (1.5 * level);
			POW += (int) ceil(0.8 * level);

			weapon = "Bow";
			atk_bonus = (int) ceil (2.4 * level);
			def_bonus = (int) ceil (1.5 * level) * -1;
			pow_bonus = (int) ceil(0.8 * level);
			break;
		case 2:                         // sword; most versatile choice, crit on 19 and 20, fatal on 48, 49, and 50. Bonuses to every stat (except resistance).
			ATK += (int) ceil (0.4 * level);
			DEF += (int) ceil (0.5 * level);
			POW += (int) ceil(0.6 * level);

			weapon = "Sword";            	   // Durendal (earth), Clarent (fire), Excalibur (water), Joyeuse (air)
			atk_bonus = (int) ceil (0.4 * level);
			def_bonus = (int) ceil (0.5 * level);
			pow_bonus = (int) ceil(0.6 * level);
			break;
	}
	
	if (ATK <= 0)
		ATK = level + 10;
	if (DEF <= 0)
		DEF = level + 8;
	//int PAB = (level-10)/2 + 3;                                <- PAB = plated armour bonus, a special ongoing bonus to the [player]'s armour class.
	//if (PAB < 0)
	//    PAB = 0;
	
	//int bubble_shield = 0;                            <- This is the variable that stores the [player]'s temporary hit points.

	//int attack_decay = level + roll (12);                <- want to test this idea later but not now
	//int fatigue;                                         <- want to implement this later too but it basically decreases the [player]'s POW value as the turn_count increases

	// Sir KNIGHT's Stats
	int k_ATK = level + 8 + roll (2);
	int k_DEF = level + 6 + roll (8);
	int k_POW = level + 7 + roll (11);
	int k_RES = level + 15 + roll (9);
	int k_HP = (int) ceil (64.1 + (1.9*level)) + roll (20,level);

	int K_DEF_ORIG = k_DEF;
	
	bool turn_flag = (bool)(roll (2) - 1);
	bool conceded = false;
	bool gameover = false;
	
	// Sir [player]'s powerups
	bool accel_guard = false;
	bool accel_strike = false;
	
	cout << "\n";
	
	// game loop
	while (!conceded && !gameover)
	{
		if (turn_flag)	// your turn
		{
			cout << "Sir " + player + "\'s turn (your turn)!" << endl;
			printOptions ();
			if (turn_count == 0 || turn_count == 1)
				cout << "Recommended that on the first turn you check the game stats." << endl;
			int option;
			cin >> option;
			cout << "Response: " << option << "\n";
			switch (option)
			{
				case 0:
					cout << "Sir " << player << " has conceded." << endl;
					conceded = true;
					break;
				case 1:
					if (selection == 0)
					{
						for (int i = 0; i < 2; i++)
						{

							bool critical = false;
							int attack_roll = roll (20);

							if (attack_roll <= 20 && attack_roll >= 18)        // crit 18-20 with daggers
								critical = true;
							attack_roll += ATK;

							int armour_class = k_DEF + 10;
							cout << "Sir " << player << " attacks Sir KNIGHT!" << endl;
							if (i == 0)
								cout << "First Strike" << endl;
							else if (i == 1)
								cout << "Second Strike" << endl;
							cout << "Your attack roll: " << (attack_roll-ATK) << "(+" << ATK << ")\tAC: " << armour_class << endl;
							if (attack_roll - ATK != 1 && (critical || attack_roll >= armour_class))
							{
								cout << "\tHit!!" << endl;
								int fatal = roll (50);
								cout << "\tFatal? " << fatal << endl;
								int damage_roll = roll (4, level) + POW;
								// Accel Strike case
								if (accel_strike)
								{     
									damage_roll += POW;                // the POW constant is used here exclusively since Accel Strike is an ability and not a normal action.
									cout << "Accel Strike! Sir " << player << " deals bonus damage!" << endl;
								}
								// Critical case (calculated beforehand)
								if (critical)                        
								{
									damage_roll *= 2;
									cout << "Sir " << player << " has just made a critical strike! x2 damage!" << endl;
								}
								// Fatal case

								if (fatal >= 46 && fatal <= 50)
								{
									damage_roll *= 5;
									cout << "Sir " << player << " has just fatally struck Sir KNIGHT! x5 damage!" << endl;
								}

								cout << "Damage roll: " << (damage_roll-POW) << "(+" << POW << ") = " << damage_roll << " damage" << endl;
								cout << "\tResist " << k_RES << " = " << (damage_roll-k_RES) << " damage" << endl;

								k_HP -= damage_roll - k_RES;
								if (damage_roll - k_RES < 0)
									cout << "Sir KNIGHT has just absorbed damage since their resistance was higher than the damage dealt!" << endl;
								cout << "Sir KNIGHT now has " << k_HP << " HP left.\n" << endl;

								if (accel_strike)
								{
									cout << "Sir " << player << " suffers severe recoil!: " << ((int)ceil(0.35*level + roll(10,level/2))) << " damage." << endl;
									HP -= (int) ceil(0.35*level + roll (10,level/2));
									cout << "Sir " << player << " now has " << HP << " HP left.\n" << endl;
								}
								int toughness = (int) ceil (0.3*K_DEF_ORIG);                    // each time the player hits the KNIGHT, the KNIGHT gets harder to hit, self-buffing his DEF value.
								k_DEF += toughness;
							}
							else
							{
								cout << "\tMiss." << endl;
								cout << "Sir KNIGHT now has " << k_HP << " HP left.\n" << endl;
								k_DEF = K_DEF_ORIG;    
							}

							if (k_HP <= 0)
							{
								cout << "Sir " << player << " victory!\t(in " << ++turn_count << " turns.)"<< endl;
								gameover = true;
								break;
							}
							else if (HP <= 0)
							{
								cout << "Sir " << player << " defeat!\t(in " << ++turn_count << " turns.)" << endl;
								conceded = true;
								break;
							}
						}
					}
					else
					{
						bool critical = false;
						int attack_roll = roll (20);

						if (selection == 1)
						{
							if (attack_roll == 20)                        // crit 20 with bow
								critical = true;
						}
						else if (selection == 2)
						{
							if (attack_roll == 19 || attack_roll == 20) // crit 19-20 with sword
								critical = true;
						}
						attack_roll += ATK;

						int armour_class = k_DEF + 10;
						cout << "Sir " << player << " attacks Sir KNIGHT!" << endl;
						cout << "Your attack roll: " << (attack_roll-ATK) << "(+" << ATK << ")\tAC: " << armour_class << endl;
						if (attack_roll - ATK != 1 && (critical || attack_roll >= armour_class))
						{
							cout << "\tHit!!" << endl;
							int fatal = roll (50);
							cout << "\tFatal?: " << fatal << endl;

							int damage_roll = 0;
							if (selection == 1)
								damage_roll = roll (8, (int)ceil(level/2.0)) + POW;
							if (selection == 2)
								damage_roll = roll (12, (int)ceil(level/3.0)) + POW;

							// Accel Strike case
							if (accel_strike)
							{     
								damage_roll += POW;
								cout << "Accel Strike! Sir " << player << " deals bonus damage!" << endl;
							}
							// Critical case (calculated beforehand)
							if (critical)                        
							{
								damage_roll *= 2;
								cout << "Sir " << player << " has just made a critical strike! x2 damage!" << endl;
							}
							// Fatal cases
							switch (selection)
							{
							case 1:
								if (fatal >= 41 && fatal <= 50)
								{
									damage_roll *= 5;
									cout << "Sir " << player << " has just fatally struck Sir KNIGHT! x5 damage!" << endl;
								}
								break;
							case 2:
								if (fatal >= 48 && fatal <= 50)
								{
									damage_roll *= 5;
									cout << "Sir " << player << " has just fatally struck Sir KNIGHT! x5 damage!" << endl;
								}
								break;
							}

							cout << "Damage roll: " << (damage_roll-POW) << "(+" << POW << ") = " << damage_roll << " damage" << endl;
							cout << "\tResist " << k_RES << " = " << (damage_roll-k_RES) << " damage" << endl;

							k_HP -= damage_roll - k_RES;
							if (damage_roll - k_RES < 0)
								cout << "Sir KNIGHT has just absorbed damage since their resistance was higher than the damage dealt!" << endl;
							cout << "Sir KNIGHT now has " << k_HP << " HP left.\n" << endl;

							if (accel_strike && selection != 1)
							{
								cout << "Sir " << player << " suffers severe recoil!: " << ((int) ceil(0.35*level + roll (6,level/2))) << " damage." << endl;
								HP -= (int) ceil(0.35*level + roll (6,level/2));
								cout << "Sir " << player << " now has " << HP << " HP left.\n" << endl;
							}
							int toughness = (int) ceil(0.3 * K_DEF_ORIG);                    // each time the player hits the KNIGHT, the KNIGHT gets harder to hit, self-buffing his DEF value.
							k_DEF += toughness;
						}
						else
						{
							cout << "\tMiss." << endl;
							cout << "Sir KNIGHT now has " << k_HP << " HP left.\n" << endl;
							k_DEF = K_DEF_ORIG;    
						}

						if (k_HP <= 0)
						{
							cout << "Sir " << player << " victory!\t(in " << ++turn_count << " turns.)" << endl;
							gameover = true;
							break;
						}
						else if (HP <= 0)
						{
							cout << "Sir " << player << " defeat!\t(in " << ++turn_count << " turns.)" << endl;
							conceded = true;
							break;
						}
					}
					
					turn_flag = false;
					turn_count++;
					accel_strike = false;
					break;
				case 2:
					accel_guard = true;
					cout << "Sir " << player << " has activated Accel Guard. Bonus Defense next turn.\n" << endl;
					
					turn_flag = false;
					turn_count++;
					break;
				case 3:
					accel_strike = true;
					cout << "Sir " << player << " has activated Accel Strike. Bonus Damage next turn.\n" << endl;
					
					turn_flag = false;
					turn_count++;
					break;
				case 4:
					cout << "Stats:" << endl;
					cout << "  Sir " << player << " Level " << level << endl;
					cout << "\tWeapon:\t" << weapon << endl;
					cout << "\tHP:\t" << HP << endl;
					cout << "\tATK:\t" << ATK << endl;
					cout << "\tDEF:\t" << DEF << endl;
					cout << "\tPOW:\t" << POW << endl;
					cout << "\tRES:\t" << RES << "\n" << endl;
					cout << "  Sir " << player << "\'s " << weapon << " Stats:" << endl;
					cout << "\tAttack Bonus:\t" << atk_bonus << endl;
					cout << "\tDefense Bonus:\t" << def_bonus << endl;
					cout << "\tPower Bonus:\t" << pow_bonus << endl;
					
					cout << "\tDamage Dice:\t";
					if (selection == 0)
						cout << level << "d6" << endl;
					else if (selection == 1)
						cout << ((int) ceil (level/2.0)) << "d8" << endl;
					else if (selection == 2)
						cout << ((int) ceil(level/3.0)) << "d12" << endl;
					
					cout << "\tCritical Strike (x2) on:\t";
					if (selection == 0)
						cout << "18, 19, and 20" << endl;
					else if (selection == 1)
						cout << "20" << endl;
					else if (selection == 2)
						cout << "19 and 20" << endl;
					
					cout << "\tFatal Strike (x5) on:\t";
					if (selection == 0)
						cout << "46, 47, 48, 49, and 50\n" << endl;
					else if (selection == 1)
						cout << "41, 42, 43, 44, 45, 46, 47, 48, 49, and 50\n" << endl;
					else if (selection == 2)
						cout << "48, 49, and 50\n" << endl;
					
					cout << "Number of turns elapsed:\t" << turn_count << "\n" << endl;
					
					break;
				default:
					cout << "I\'m sorry, but the program could not recognise your input. Please reenter a number corresponding to the action you would like to take this turn.\n" << endl;
			}
		}
		else		// opponent turn
		{
			cout << "Sir KNIGHT\'s turn (enemy turn)." << endl;
			
			bool critical = false;
			int attack_roll = roll (20);
			if (attack_roll == 20 || attack_roll == 19)			// 10% crit chance
				critical = true;
			attack_roll += k_ATK;
			
			int armour_class = DEF + 10;
			cout << "Sir KNIGHT attacks Sir " << player << "!" << endl;
			cout << "Sir KNIGHT\'s attack roll: " << (attack_roll-k_ATK) << "(+" << k_ATK << ")\tAC: " << armour_class << endl;
			if (attack_roll - k_ATK != 1 && (attack_roll >= armour_class || critical))
			{
				cout << "\tHit!!" << endl;
				int damage_roll = roll (10, (int) ceil (level/3.0)) + k_POW;
				
				if (accel_guard)
				{
					damage_roll -= RES + level;
					cout << "Accel Guard! Sir " << player << " gets bonus resistance!" << endl;
				}
				if (critical)
				{
					damage_roll *= 3;			// the enemy KNIGHT's power is not as strong as the [player]'s, but he has more power in his critical strike.
					cout << "Sir KNIGHT has just made a critical strike!" << endl;
				}
				cout << "Damage roll: " << (damage_roll-k_POW) << "(+" << k_POW << ") = " << damage_roll << " damage" << endl;
				cout << "\tResist " << RES << " = " << (damage_roll-RES) << " damage" << endl;
				
				HP -= damage_roll - RES;
				if (damage_roll - RES < 0)
					cout << "Sir " << player << " has just absorbed damage since their resistance was higher than the damage dealt!" << endl;
				cout << "Sir " << player << " now has " << HP << " HP left.\n" << endl;
	
			}
			else
			{
				cout << "\tMiss." << endl;
				cout << "Sir " << player << " now has " << HP << " HP left.\n" << endl;
			}
			
			if (HP <= 0)
			{
				cout << "Sir " << player << " defeat!\t(in " << ++turn_count << " turns.)" << endl;
				conceded = true;							// to make it easier to determine what "gameover screen" to display to the console
			}
			
			turn_flag = true;
			accel_guard = false;
			turn_count++;
		}
	} // end of game loop
	
	cout << "Press any key then return to exit: ";
	string ret; cin >> ret;
	
	return 0;
}