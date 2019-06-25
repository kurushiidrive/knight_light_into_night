#pragma once
#include <string>
#include <cmath>
#include <vector>
#include "Die.h"

class Player
{
public:
	Player();
	bool Crit(int roll); // determines if said roll is a crit, based on profession.
	bool Fatal(int roll); // determines if said roll is a fatal, based on profession.
	bool Switch ();	// applies only to ranger.

	bool Accel_Strike();
	bool Accel_Guard();
	void Set_Accel_Guard(bool);

	std::vector<int> Use_Normal_Ability();

	void Set_Name(std::string nm);
	std::string Get_Name();

	int Get_Prof();
	std::string Get_Weapon();

	int Get_Level();
	void Set_Level(int lvl);

	void Reset_Action_Points();
	int Get_Action_Points();

	int Get_ATK();
	void Set_ATK(int);
	int Get_DEF();
	void Set_DEF(int);
	int Get_POW();
	void Set_POW(int);
	int Get_RES();
	void Set_RES(int);

	int Get_HP();
	void Set_HP(int);

	int Get_Atk_Bonus();
	void Set_Atk_Bonus(int);
	int Get_Def_Bonus();
	void Set_Def_Bonus(int);
	int Get_Pow_Bonus();
	void Set_Pow_Bonus(int);

	void Reset(int);

private:
	int ATK, DEF, POW, RES;	// attack, defense, power, and resistance
	int level;
	int HP;

	int BASE_ACTION_POINTS;
	int action_points;

	std::string weapon;
	int atk_bonus, def_bonus, pow_bonus;
	bool accel_guard, accel_strike;

	Die die;
	enum profession {
		WARRIOR = 0, RANGER = 1, WIZARD = 2
	} prof;

	void set(int level, profession = WARRIOR);

	bool initialised = false;

	std::string name;
};