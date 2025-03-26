#include "MyUtilities.h"
#include "Graphics.h"

int MyUtilities::randomBetween(const int a, const int b) {
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> rand(a, b);

	return rand(rng);
}