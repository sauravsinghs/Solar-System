#include <iostream>
#include "Game.h"
#include "Settings.h"  //  setting.h mein constanst define hote hai jaise widow size and camera speed and texture paths etc.

int main()
{
	using namespace settings;
	Game game{ windowWidth, windowHeight, viewportX, viewportY, viewportWidth, viewportHeight, windowTitle };

	while (!game.ShouldClose())
	{
		game.Tick();
	}
	return 0;
}
// Yahan se main game loop start hota hai.

// 🔹 game.ShouldClose()

// Ye Window class ke ShouldClose() function ko call karta hai.

// Ye check karta hai ki user ne window close button dabaya hai ya exit key (ESC) press ki hai.

// Jab ye true hota hai → loop break ho jata hai aur game band ho jata hai.

// 🔹 game.Tick()

// Har frame mein yeh function call hota hai.
// Ye do main functions ko internally call karta hai:

// Update(float deltatime)

// User input handle karta hai (keyboard/mouse)

// Camera movement update karta hai

// Planets ko unke orbit ke hisaab se rotate aur translate karta hai

// Time speed aur pause state manage karta hai

// Draw(float deltatime)

// Camera ka view aur projection matrix banata hai

// Sun, Earth, Mars, aur Skybox draw karta hai using OpenGL calls

// Shaders ke uniforms set karta hai aur Window.DrawActor() se render karta hai

// So overall:

// 🔁 Tick() = Update() + Draw()