#include "Night.h"
#include <cmath>

Night::Night()
{
	set();
}

void Night::set(int lvl)
{
	level = lvl;	// for debugging make it and PC's level |1|.
	n_ATK = level + 8 + die.Reset(1, 2)(level/2+1);
	n_DEF = level + 6 + die.Reset(1, 8)(level/2+1);
	n_POW = level + 7 + die.Reset(1, 11)(level/2+1);
	n_RES = level + 5 + die.Reset(1, 9)(level/2+1);
	n_HP = (int)std::ceil(64.1 + (1.9 * level)) + die.Reset(1, 20)(level);

	BASE_ACTION_POINTS = level / 10;
	action_points = BASE_ACTION_POINTS;
	special_ability_cooldown = 0;	// not using this yet
	toughness = 0;					// for now
	N_DEF_ORIG = 0;					// for now
}

std::vector<int> Night::Use_Normal_Ability()
{
	// contains information on the results of using the ability
	// first item is the attack roll (including attack bonuses)
	// second is whether it crit (0 or 1)
	// third is damage roll if it will hit
	std::vector<int> ret;

	if (action_points < 1)
		return ret;

	bool critical = false;
	int attack_roll = die.Reset(1, 20)();
	if (attack_roll == 20 || attack_roll == 19) // 10% crit chance
		critical = true;
	attack_roll += n_ATK;

	// Night aren't as powerful as the Player Character Knights, but they have much more power in their critical strikes.
	int damage_roll = die.Reset(1, 10)((int)std::ceil(level / 3.0)) + n_POW;
	if (critical)
		damage_roll *= 3;
	
	ret.push_back(attack_roll);
	ret.push_back(critical);
	ret.push_back(damage_roll);

	action_points--;
	return ret;
}

int Night::Get_ATK()
{
	return n_ATK;
}

void Night::Set_ATK(int a)
{
	n_ATK = a;
}

int Night::Get_DEF()
{
	return n_DEF;
}

void Night::Set_DEF(int d)
{
	n_DEF = d;
}

int Night::Get_POW()
{
	return n_POW;
}

void Night::Set_POW(int p)
{
	n_POW = p;
}

int Night::Get_RES()
{
	return n_RES;
}

void Night::Set_RES(int r)
{
	n_RES = r;
}

int Night::Get_HP()
{
	return n_HP;
}

void Night::Set_HP(int h)
{
	n_HP = h;
}

int Night::Get_Toughness()
{
	return toughness;
}

void Night::Set_Toughness(int t)
{
	toughness = t;
}

void Night::Reset_Action_Points()
{
	action_points = BASE_ACTION_POINTS;
}

void Night::Reset()
{
	set(level);
}