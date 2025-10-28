// Entry point: creates the Game and runs the frame loop until the window requests close.
#include <iostream>
#include "Game.h"
#include "Settings.h"

int main()
{
	using namespace settings;
	Game game{ windowWidth, windowHeight, viewportX, viewportY, viewportWidth, viewportHeight, windowTitle };

	// Main loop: update and render a frame until the user closes the window.
	while (!game.ShouldClose())
	{
		game.Tick();
	}
	return 0;
}