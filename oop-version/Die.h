#pragma once
#ifdef _MSC_VER
#undef max
#endif

#include <limits>
#include <random>
#include <functional>

class Die
{
public:
	Die(int lo = 0, int hi = std::numeric_limits<int>::max()) :
		p{ lo, hi },
		r{ std::bind(std::uniform_int_distribution<>{p}, std::mt19937{(std::random_device{}).operator()()}) },
		lowerbound_{ lo },
		upperbound_{ hi }
	{}
	int operator() (int times = 1) const;	// roll the die
	int GetLowerBound() const noexcept { return lowerbound_; }
	int GetUpperBound() const noexcept { return upperbound_; }
	Die& Reset(int lo = 0, int hi = std::numeric_limits<int>::max());

private:
	std::uniform_int_distribution<>::param_type p; // used for setting the highest and lowest possible rolls on the die
	std::function<int()> r;							// function object for rolling
	int lowerbound_, upperbound_;
};