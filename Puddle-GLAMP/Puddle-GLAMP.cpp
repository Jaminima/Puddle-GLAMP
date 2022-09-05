#include <iostream>
#include "App.h"

int main(int argc, char** argv)
{
	std::cout << "Hello World!\n";
	InitWorld();
	SetupFrame(argc, argv);

	while (true) {}
}