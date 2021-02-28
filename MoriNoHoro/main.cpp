#include "gameEngine.h"

// game initialization
int main()
{
	MoriNoHoro::gameEngine engine;

	// check if engine was initialized properly
	if (engine.init())
		// start update loop
		engine.run();

	return 0;
}