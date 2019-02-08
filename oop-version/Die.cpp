#include "Die.h"
#include <random>

int Die::operator() (int times) const
{
	int sum = 0;
	for (int i = 0; i < times; i++)
		sum += r();

	return sum;
}

Die& Die::Reset(int lo, int hi)
{
	p = std::uniform_int_distribution<>::param_type{ lo, hi };
	r = std::function<int()>{ std::bind(std::uniform_int_distribution<>{p}, std::mt19937{(std::random_device{}).operator()()}) };

	return *this;
}