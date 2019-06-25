#pragma once
#include "Die.h"
#include <vector>
class Night
{
public:
	Night();
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

	int Get_Toughness();
	void Set_Toughness(int);

	void Reset_Action_Points();

	std::vector<int> Use_Normal_Ability();

	void Reset();

protected:
	Die die;
	int level;
	int N_DEF_ORIG;

private:
	int n_ATK, n_DEF, n_POW, n_RES;
	int n_HP;

	int BASE_ACTION_POINTS;	// number of actions in a turn (normal abilities take one action, specials take two)
	int action_points;
	int special_ability_cooldown;
	int toughness;

	void set(int = 10);
};