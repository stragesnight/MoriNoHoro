#include "gameEngine.h"

// game initialization
int main()
{
	MoriNoHoro::gameEngine engine;

	int seed;
	std::cout << "STRAGESNIGHT'S GPU-DRIVEN 3D CELLULAR AUTOMATA\n\n";
	std::cout << " - Controls:\n\tWASD - move;\n\tQ, E - change elevation;\n\tSPACE - progress simulation;\n<buggy>\tctrl + up/down arrow - change view angle.";
	std::cout << "\n\n = If something feels wrong or simply does not work, please contact me! =";
	std::cout << "\n\nEnter generation seed (any number): ";
	std::cin >> seed;

	// check if engine was initialized properly
	if (engine.init(seed))
		// start update loop
		engine.run();

	return 0;
}