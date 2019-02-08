#pragma once
#include <string>
#include <cmath>
#include "Die.h"

class Player
{
public:
	Player();
	bool Crit(int roll); // determines if said roll is a crit, based on profession.
	bool Fatal(int roll); // determines if said roll is a fatal, based on profession.
	bool Switch ();	// applies only to ranger.

private:
	int ATK, DEF, POW, RES;	// attack, defense, power, and resistance
	int level;
	int HP;

	std::string weapon;
	int atk_bonus, def_bonus, pow_bonus;

	Die die;
	enum profession {
		WARRIOR = 0, RANGER = 1, WIZARD = 2
	} prof;

	void set(int level);

	bool initialised = false;
};