#include "Player.h"
//#define debug_
#ifdef debug_
#include <iostream>
#endif

Player::Player() :
	level{ 1 }
{
	set(level);
	initialised = true;
}

void Player::set(int level, profession p)
{
	// note to self: when levelling up, use different calculation for stats so that roll bonuses between levels aren't lost
	ATK = level + 10 + die.Reset(1, 8)(level);
	DEF = level + 8 + die.Reset(1, 6)(level);
	POW = level + 12 + die(level);
	RES = level + 20 + die.Reset(1, 2)(level);
	HP = (int)std::ceil(48.1 + (1.29 * level));

	prof = p;

	// for these base weapons below, note that the atk/def/pow bonuses are the same as the professions base bonuses to the base ATK/DEF/POW stats themselves.
	// exception lies in Ranger's daggers, where the atk/def/pow bonuses are adjusted more than normal to eliminate the base bonuses on the bow.
	switch (prof)
	{
	case WARRIOR:
		weapon = "longsword";	// Durendal (earth), Clarent (fire), Excalibur (water), Joyeuse (air)
		// longsword; very versatile, crit on 19 and 20, fatal on 48-50. Bonuses to every stat except RES.
		ATK += (int)std::ceil(1.4 * level);
		DEF += (int)std::ceil(1.5 * level);
		POW += (int)std::ceil(1.6 * level);

		HP += 4 * die.Reset(1, 12)(level);

		atk_bonus = (int)std::ceil(1.4 * level);
		def_bonus = (int)std::ceil(1.5 * level);
		pow_bonus = (int)std::ceil(1.6 * level);
		break;
	case RANGER:
		weapon = "bow daggers";
		// by bow
		ATK += (int)std::ceil(3.4 * level);
		DEF -= (int)std::ceil(2.5 * level);
		POW += (int)std::ceil(2.8 * level);

		HP += 2 * die.Reset(1, 8)(level);

		if (weapon.substr(0, weapon.find(' ')) == "bow")	// bow; easier to hit, but harder to defend (since bow occupies 2H); fatals very easily (41-50), but since good aim is necessary, it won't always crit. Accel Strike possible without recoil.
		{
			atk_bonus = (int)std::ceil(3.4 * level);
			def_bonus = 0;
			pow_bonus = (int)std::ceil(2.8 * level);
		}
		else if (weapon.substr(0, weapon.find(' ')) == "daggers") // daggers; crits easily since it's a precision weapon (18-20). Also fatals relatively well (46-50).
		{
			atk_bonus = ((int)std::ceil(3.0 * level));
			def_bonus = (int)std::ceil(3.6 * level);
			pow_bonus = -((int)std::ceil(2.0 * level));
		}
		break;
	case WIZARD:
		weapon = "staff";
		// staff crits on 20 (but for no additional damage, only a guaranteed hit). cannot fatal. 
		ATK += (int)std::ceil(1.7 * level);
		DEF -= (int)std::ceil(2.0 * level);
		POW += (int)std::ceil(3.1 * level);

		HP += die.Reset(1, 6)(level);
		RES += (int)std::ceil(2.0 * level);

		// wizard is the only profession that can self-buff RES without using items.
		atk_bonus = (int)std::ceil(1.7 * level);
		def_bonus = 0;
		pow_bonus = (int)std::ceil(3.1 * level);

		break;
	}

	if (ATK <= 0)
		ATK = level + 10;
	if (DEF <= 0)
		DEF = level + 8;

	BASE_ACTION_POINTS = level / 5;
	if (BASE_ACTION_POINTS < 1)
		BASE_ACTION_POINTS = 1;	// minimum 1 action points
	if (prof == RANGER && weapon.substr(0, weapon.find(' ')) == "daggers")
		BASE_ACTION_POINTS *= 2;
	action_points = BASE_ACTION_POINTS;
}

bool Player::Crit(int roll)
{
	bool result = false;

	switch (prof)
	{
	case WARRIOR:
		if (roll == 19 || roll == 20)
			result = true;
		break;
	case RANGER:
		if (weapon.substr(0, weapon.find(' ')) == "daggers")
		{
			if (roll >= 18 && roll <= 20)
				result = true;
		}
		else if (weapon.substr(0, weapon.find(' ')) == "bow")
		{
			if (roll == 20)
				result = true;
		}
		break;
	case WIZARD:
		if (roll == 20)
			result = true;
		break;
	}

	return result;
}

bool Player::Fatal(int roll)
{
	bool result = false;

	switch (prof)
	{
	case WARRIOR:
		if (roll >= 48 && roll <= 50)
			result = true;
		break;
	case RANGER:
		if (weapon.substr(0, weapon.find(' ')) == "daggers")
		{
			if (roll >= 46 && roll <= 20)
				result = true;
		}
		else if (weapon.substr(0, weapon.find(' ')) == "bow")
		{
			if (roll >= 41 && roll <= 50)
				result = true;
		}
		break;
	case WIZARD:
		break;
	}

	return result;
}

bool Player::Switch()
{
	if (prof == WARRIOR || prof == WIZARD)
		return false;

	if (action_points < 1)
		return false;

	//	std::size_t index = weapon.find(' ');
	if (weapon == "bow daggers")
	{
		// make sure proper combat bonuses are applied
		atk_bonus = ((int)std::ceil(3.0 * level));
		def_bonus = (int)std::ceil(3.6 * level);
		pow_bonus = -((int)std::ceil(2.0 * level));

		BASE_ACTION_POINTS *= 2;
		action_points = BASE_ACTION_POINTS;
		weapon = "daggers bow";
	}
	else if (weapon == "daggers bow")
	{
		// make sure proper combat bonuses are applied

		atk_bonus = (int)std::ceil(3.4 * level);
		def_bonus = 0;
		pow_bonus = (int)std::ceil(2.8 * level);

		BASE_ACTION_POINTS /= 2;
		action_points = BASE_ACTION_POINTS;
		weapon = "bow daggers";
	}

	return true;
}

bool Player::Accel_Strike()
{
	if (action_points < 1 || accel_strike)	// if not enough AP or you're already in accel strike (to change second condition later to accomodate for dual-wielding)
		return false;

	accel_strike = true;
	action_points--;
	return true;
}

bool Player::Accel_Guard()
{
	if (action_points < 1 || accel_guard)	// if not enough AP or you're already in accel guard
		return false;

	accel_guard = true;
	action_points--;
	return true;
}

void Player::Set_Accel_Strike(bool t)
{
	accel_strike = t;
}

bool Player::Get_Accel_Strike()
{
	return accel_strike;
}

void Player::Set_Accel_Guard(bool t)
{
	accel_guard = t;
}

bool Player::Get_Accel_Guard()
{
	return accel_guard;
}

std::vector<int> Player::Use_Normal_Ability()
{
	// contains information on the results of using the ability
	// first item is the attack roll (including attack bonuses)
	// second is whether it crit (0 or 1)
	// third is damage roll if it will hit
	// fourth is whether it fataled (0 or 1)
	// fifth is accel strike flag (0 or 1)
	// sixth is recoil from accel strike (if applicable)
	std::vector<int> ret;

	if (action_points < 1)
		return ret;

	bool critical;
	int attack_roll = die.Reset(1, 20)();
	critical = Crit(attack_roll);
	attack_roll += ATK + atk_bonus;

	int damage_roll = POW + pow_bonus;
	switch (prof)
	{
	case WARRIOR:
		damage_roll += die.Reset(1, 10)((int)std::ceil(level / 2.5));
		break;
	case RANGER:
		if (weapon.substr(0, weapon.find(' ')) == "daggers")
			damage_roll += die.Reset(1, 4)(level);
		else if (weapon.substr(0, weapon.find(' ')) == "bow")
			damage_roll += die.Reset(1, 8)((int)std::ceil(level / 2.0));
		break;
	case WIZARD:
		damage_roll += die.Reset(1, 12)((int)std::ceil(level * 1.5));
		break;
	}

	bool fatal = Fatal(die.Reset(1, 50)());
	bool ac_str = accel_strike;
	int recoil = 0;
	if (ac_str)
	{
		damage_roll += POW;
		accel_strike = false;
		if (prof != RANGER || (prof == RANGER && weapon.substr(0, weapon.find(' ')) != "bow"))
		{
			recoil = (int)std::ceil(0.35 * level + die.Reset(1, 6)(level / 2));
			HP -= recoil;	// accel strike recoil
		}
	}

	ret.push_back(attack_roll);
	ret.push_back(critical);
	ret.push_back(damage_roll);
	ret.push_back(fatal);
	ret.push_back(ac_str);
	if (ac_str && recoil != 0)
		ret.push_back(recoil);

	action_points--;
	return ret;
}

void Player::Set_Name(std::string nm)
{
	name = nm;
}

std::string Player::Get_Name()
{
	return name;
}

int Player::Get_Prof()
{
	return prof;
}

std::string Player::Get_Weapon()
{
	return weapon;
}

int Player::Get_Level()
{
	return level;
}

void Player::Set_Level(int lvl)
{
	level = lvl;
	set(level, prof);
}

void Player::Reset_Action_Points()
{
	action_points = BASE_ACTION_POINTS;
}

int Player::Get_Action_Points()
{
	return action_points;
}

int Player::Get_ATK()
{
	return ATK;
}

void Player::Set_ATK(int a)
{
	ATK = a;
}

int Player::Get_DEF()
{
	return DEF;
}

void Player::Set_DEF(int d)
{
	DEF = d;
}

int Player::Get_POW()
{
	return POW;
}

void Player::Set_POW(int p)
{
	POW = p;
}

int Player::Get_RES()
{
	return RES;
}

void Player::Set_RES(int r)
{
	RES = r;
}

int Player::Get_HP()
{
	return HP;
}

void Player::Set_HP(int h)
{
	HP = h;
}

int Player::Get_Atk_Bonus()
{
	return atk_bonus;
}

void Player::Set_Atk_Bonus(int ab)
{
	atk_bonus = ab;
}

int Player::Get_Def_Bonus()
{
	return def_bonus;
}

void Player::Set_Def_Bonus(int db)
{
	def_bonus = db;
}

int Player::Get_Pow_Bonus()
{
	return pow_bonus;
}

void Player::Set_Pow_Bonus(int pb)
{
	pow_bonus = pb;
}

void Player::Reset(int p)
{
	set(level, static_cast<profession>(p));
}