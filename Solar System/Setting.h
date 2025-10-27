#pragma once
#include <string>
#include "glm/glm.hpp"
//Settings that can be tweaked.
namespace settings
{
	//Directory paths.
	const std::string texturesPath = "..\\Resources\\Textures\\";
	const std::string meshesPath = "..\\Resources\\Meshes\\";
	const std::string shadersPath = "..\\Resources\\Shaders\\";

    // Note: project is intentionally minimal (Sun, Earth, Mars, skybox)

	//Simulation settings.
	//Lighting.
	constexpr glm::vec3 sunlightColor = {1.f,1.f,1.f};//The color that the sun casts on the planets.
	constexpr glm::vec3 ambientColor = {0.05f,0.05f,0.05f};//The ambient color of all planets (except earth).
	constexpr glm::vec3 earthAmbientColor = {0.7f,0.7f,0.7f};//The ambient color of the earth.
	constexpr float earthSpecularStrength = 1.5f;//Controls the strength of the specular of the ware's surface of earth.
	constexpr float earthSpecularShininess = 4;	//Controls the shininess of the water's surface of earth (powers of 2).
	//Radiuses.
	constexpr float earthOrbitRadius = 300.0f;		//The radius of the earth around the sun (other planet's radiuses are scaled based on this value).
	constexpr float mercuryOrbitRadius = earthOrbitRadius * 0.6f;
	constexpr float venusOrbitRadius = earthOrbitRadius * 0.8f;
	constexpr float marsOrbitRadius = earthOrbitRadius * 1.3f;
	constexpr float jupiterOrbitRadius = earthOrbitRadius * 2.0f;
	constexpr float saturnOrbitRadius = earthOrbitRadius * 3.0f;
	constexpr float uranusOrbitRadius = earthOrbitRadius * 3.8f;
	constexpr float neptuneOrbitRadius = earthOrbitRadius * 4.3f;

	//Scales.
	constexpr float sunScale = 100.f;				
	constexpr float earthScale = 10.f;				//The scale of the earth (other planet's scales are adjusted based on this value).
	constexpr float mercuryScale = earthScale * 0.3f;
	constexpr float venusScale = earthScale * 0.9f;	
	constexpr float marsScale = earthScale * 0.5f;	
	constexpr float jupiterScale = earthScale * 5.0f;
	constexpr float saturnScale = earthScale * 4.0f;	
	constexpr float uranusScale = earthScale * 2.5f;	
	constexpr float neptuneScale = earthScale * 2.3f;

	//Orbit speeds (in degrees per second).
	constexpr float earthOrbitSpeed = 20.f;	//The number of degrees the earth should travel around its orbit every second (other planet's orbit speeds are adjusted based on this value).
	constexpr float mercuryOrbitSpeed = earthOrbitSpeed * 4.149f;
	constexpr float venusOrbitSpeed = earthOrbitSpeed * 1.623f;
	constexpr float marsOrbitSpeed = earthOrbitSpeed * 0.531f;
	constexpr float jupiterOrbitSpeed = earthOrbitSpeed * 0.084f;
	constexpr float saturnOrbitSpeed = earthOrbitSpeed * 0.033f;
	constexpr float uranusOrbitSpeed = earthOrbitSpeed * 0.011f;
	constexpr float neptuneOrbitSpeed = earthOrbitSpeed * 0.006f;

	//Rotation Speeds (in degrees per second).
	constexpr float earthRotationSpeed = 50.f;
	constexpr float sunRotationSpeed = earthRotationSpeed * 0.037f;	//The number of degrees the earth should rotate around itself every second (other planet's rotation speeds are adjusted based on this value).
	constexpr float mercuryRotationSpeed = earthRotationSpeed * 17.241f;
	constexpr float venusRotationSpeed = -earthRotationSpeed * 4.115f;	//Rotates the other way compared to earth.
	constexpr float marsRotationSpeed = earthRotationSpeed * 0.960f;
	constexpr float jupiterRotationSpeed = earthRotationSpeed * 2.403f;
	constexpr float saturnRotationSpeed = earthRotationSpeed * 2.183f;
	constexpr float uranusRotationSpeed = -earthRotationSpeed * 1.412f;	//Rotates the other way compared to earth.
	constexpr float neptuneRotationSpeed = earthRotationSpeed * 1.501f;

	//Time settings.
	constexpr float timeAdjustSpeed = 0.1f;

	// Moon (approximate, scaled to simulation units)
	constexpr float moonOrbitRadius = earthScale * 3.5f; // keep near Earth in this simulation scale
	constexpr float moonScale = earthScale * 0.27f;
	constexpr float moonOrbitSpeed = earthOrbitSpeed * 13.0f; // ~13x per Earth year (approximate monthly)
	constexpr float moonRotationSpeed = moonOrbitSpeed; // tidally locked approximation

	// Saturn rings (relative to Saturn scale)
	constexpr float saturnRingInner = 1.2f; // inner radius multiplier of Saturn radius
	constexpr float saturnRingOuter = 2.4f; // outer radius multiplier of Saturn radius
	constexpr float saturnRingTiltDegrees = 26.7f; // axial tilt
	constexpr float saturnRingAngularSpeed = 25.0f; // degrees per second for animated particle-like motion
	constexpr float saturnRingDifferential = 0.8f; // inner faster than outer (0..1)

	// Venus atmosphere
	constexpr float venusAtmosphereScale = 1.03f; // slightly larger than Venus sphere
	constexpr float venusAtmosphereRotation = -earthRotationSpeed * 1.0f; // super-rotation approximation

	// Asteroid belt (between Mars and Jupiter, approximate)
	constexpr int asteroidCount = 1000;
	constexpr float asteroidBeltInner = marsOrbitRadius + marsScale * 8.0f; // wider safe margin from Mars
	constexpr float asteroidBeltOuter = jupiterOrbitRadius - jupiterScale * 2.0f; // safe margin from Jupiter
	constexpr float asteroidBeltTiltDegrees = 2.0f; // small inclination
	constexpr float asteroidBeltHalfThickness = earthScale * 1.5f; // vertical half-thickness
	constexpr float asteroidMinScale = earthScale * 0.08f;
	constexpr float asteroidMaxScale = earthScale * 0.22f;
	constexpr float asteroidBaseAngularSpeed = earthOrbitSpeed * 0.35f; // inner band speed benchmark
	constexpr float asteroidSpeedExponent = 1.5f; // Kepler-like

	//Keyboard bindings (US layout, use https://www.glfw.org/docs/3.3/group__keys.html for looking up the key's values).
	constexpr int forwardKey = 87;		//W
	constexpr int leftKey = 65;			//A
	constexpr int backwardKey = 83;		//S
	constexpr int rightKey = 68;			//D
	constexpr int upKey1 = 69;			//E
	constexpr int upKey2 = 32;			//Space
	constexpr int downKey = 81;			//Q
	constexpr int wireframeModeKey = 84;	//T
	constexpr int cameraSpeedupKey = 86;	//V
	constexpr int cameraSlowdownKey = 67;//C
	constexpr int pauseKey = 80;			//P
	constexpr int timeSpeedupKey = 334;	//Keypad ADD
	constexpr int timeSlowdownKey = 333;	//Keypad SUBTRACT
	constexpr int exitKey = 256;			//Escape

	//Default window values.
	constexpr int windowWidth = 1280;				//The width of the window.
	constexpr int windowHeight = 720;				//The height of the window.
	constexpr int viewportX = 0;						//The left x-coordinate of the viewport rectangle (from left of window).
	constexpr int viewportY = 0;						//The bottom y-coordinate of the viewport rectangle (from bottom of window).
	constexpr int viewportWidth = 1280;				//The width of the viewport rectangle (from left of window).
	constexpr int viewportHeight = 720;				//The height of the viewport rectangle (from bottom of window).
	constexpr char windowTitle[] = "Solar system";	//The title of the window.
	constexpr bool captureMouse = true;				//Whether or not the window should capture the mouse.

	//Default camera values.
	constexpr glm::vec3 cameraInitialPosition = { 0.0f, 0.0f, 1000.0f }; //The initial position of the camera in world space.
	constexpr float cameraYaw = -90.0f;				//The yaw of the camera.
	constexpr float cameraPitch = 0.0f;				//The pitch of the camera.
	constexpr float cameraSpeed = 100.0f;			//The speed of the camera's movement.
	constexpr float cameraSensitivity = 0.05f;		//The sensitivity of the mouse.
	constexpr float cameraFOV = 45.0f;				//The field of view.
	constexpr float cameraNearPlaneDistance = 0.3f;	//The distance of the near plane from the camera.
	constexpr float cameraFarPlaneDistance = 10000.0f;//The distance of the far plane from the camera.
	constexpr float cameraMaxPitch = 89.0f;			//Sets the camera's maximum pitch (when looking up and down).
	constexpr float cameraSpeedupRate = 1.0f;		//The amount of speed that gets added/subtracted when the camera speeds up or slows down.

	//Precalculate some values for convenience.
	constexpr float screenRatio = float(windowWidth) / float(windowHeight);

	//Check that the settings make sense.
	static_assert(windowWidth > 0 && windowHeight > 0);		//Window width/height must be positive non-zeroes.
	static_assert(viewportWidth > 0 && viewportHeight > 0);	//Viewport width/height must be positive non-zeroes.
	static_assert(cameraNearPlaneDistance > 0.0f);			//Near plane distance must be a positive non-zero.
	static_assert(cameraNearPlaneDistance < cameraFarPlaneDistance);//Near plane must be "nearer" than the far plane.
	static_assert(cameraSensitivity > 0.0f);				//Sensitivity must be a positive non-zero.
}