#include "Player.h"

Player::Player() :
	level{ 1 }
{
	set(level);
	initialised = true;
}

void Player::set(int level)
{
	ATK = level + 10 + die.Reset(1, 8)(level);
	DEF = level + 8 + die.Reset(1, 6)(level);
	POW = level + 12 + die(level);
	RES = level + 20 + die.Reset(1, 2)(level);
	HP = (int)std::ceil(48.1 + (1.29*level));

	if (!initialised)
		prof = static_cast<profession>(die.Reset(0, 2)());

	switch (prof)
	{
	case WARRIOR:
		weapon = "longsword";	// Durendal (earth), Clarent (fire), Excalibur (water), Joyeuse (air)
		// longsword; very versatile, crit on 19 and 20, fatal on 48-50. Bonuses to every stat except RES.
		ATK += (int)ceil(0.4 * level);
		DEF += (int)ceil(0.5 * level);
		POW += (int)ceil(0.6 * level);

		HP += 4 * die.Reset(1, 12)(level);

		atk_bonus = (int)ceil(0.4 * level);
		def_bonus = (int)ceil(0.5 * level);
		pow_bonus = (int)ceil(0.6 * level);
		break;
	case RANGER:
		weapon = "bow daggers";
		// by bow
		ATK += (int)ceil(2.4 * level);
		DEF -= (int)ceil(1.5 * level);
		POW += (int)ceil(0.8 * level);

		HP += 2 * die.Reset(1, 8)(level);

		if (weapon.substr(0, weapon.find(' ')) == "bow")	// bow; easier to hit, but harder to defend (since bow occupies 2H); fatals very easily (41-50), but since good aim is necessary, it won't always crit. Accel Strike possible without recoil.
		{
			atk_bonus = (int)ceil(2.4 * level);
			def_bonus = 0;
			pow_bonus = (int)ceil(0.8 * level);
		}
		else if (weapon.substr(0, weapon.find(' ')) == "daggers") // daggers; crits easily since it's a precision weapon (18-20). Also fatals relatively well (46-50).
		{
			atk_bonus = 0;
			def_bonus = (int)ceil(2.1 * level);
			pow_bonus = (int)ceil(1.0 * level);
		}
		break;
	case WIZARD:
		weapon = "staff";
		// staff crits on 20 (but for no additional damage, only a guaranteed hit). cannot fatal. 
		ATK += (int)ceil(0.7 * level);
		DEF -= (int)ceil(1.0 * level);
		POW += (int)ceil(2.1 * level);

		HP += die.Reset(1, 6)(level);

		// wizard is the only profession that can self-buff RES without using items.
		
		break;
	}
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
	if (prof == WARRIOR || prof == RANGER)
		return false;

	int index = weapon.find(' ');
}