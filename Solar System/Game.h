#pragma once
#include <string>
#include "Window.h"
#include "ShaderProgram.h"
#include "Actor.h"
#include "Planet.h"
#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"
#include <vector>
//A class that represents the game simulation and handles the logic/render loop.
class Game
{
public:
	Game(int windowWidth, int windowHeight, int viewportX, int viewportY, int viewportWidth, int viewportHeight,
		const std::string title, struct GLFWmonitor* monitor = nullptr, struct GLFWwindow* share = nullptr);
	void Tick();				//Updates the game and draws a frame.
	bool ShouldClose() const;	// check that window is close or not
	void Update(float deltatime);
	void Draw(float deltatime);
private:
	Window window;	//Window must be constructed first, it initializes OpenGL context and GLFW window.
private:
    ShaderProgram defaultShader; // planets ke liye 
    ShaderProgram noLightShader;  // sun and skybox ke liye
	Camera camera; // camera object controls the view and projection matrices

    Mesh sphereMesh;  // 3d geometry for planets
    std::vector<Texture> planetTextures; // textures for the planets
    std::vector<Planet> planets; // planets in the solar system
    Actor skyBox;  // skybox surrounding the scene
    Texture skyboxTexture;  // starry background texture
	glm::vec2 lastMousePosition;
    float lastTime;
    float shaderTime = 0.0f; // Animation timing variable
    float timeSpeed = 1.0f;
    bool isPaused = false;  // pause control
};