/**
 * \file Main.cpp
 * \brief File containing the main function
 */

#include "DummyHeader.h"
#include "AllocatorUtility.h"

int main() {
	dummy::f();

	Block *b = nullptr;

	std::cout << b << std::endl;
	b++;
	std::cout << b << std::endl;

	return 0;
}
