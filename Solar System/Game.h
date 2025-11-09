#pragma once
#include <string>
#include "Window.h"
#include "ShaderProgram.h"
#include "Actor.h"
#include "Planet.h"
#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"
#include "Orbital.h"
#include <vector>
// Coordinates initialization, per-frame update, and rendering of the solar system.
// Owns window/GL context, shaders, meshes, textures, camera and planet state.
// Handles input (movement/time controls) and draws opaque bodies, overlays, and skybox.
//A class that represents the game simulation and handles the logic/render loop.
class Game
{
public:
	Game(int windowWidth, int windowHeight, int viewportX, int viewportY, int viewportWidth, int viewportHeight,
		const std::string title, struct GLFWmonitor* monitor = nullptr, struct GLFWwindow* share = nullptr);
	void Tick();				//Updates the game and draws a frame.
	bool ShouldClose() const;	//Returns whether or not the window is flagged for closing.
private:
	struct FollowTargetEntry;
	void Update(float deltatime);
	void Draw(float deltatime);
	void InitializeFollowTargets();
	void CycleFollowTarget();
	void UpdateFollowCamera();
	void UpdateFollowHud(const FollowTargetEntry& entry);
	void DrawFollowHud();
	void InitializeHudResources();
	void InitializeOrbitPaths();
	void DrawOrbitPaths(const glm::mat4& projection, const glm::mat4& viewMatrix);
	glm::vec3 GetPlanetWorldPosition(size_t index) const;
	glm::vec3 GetFollowTargetPosition(const FollowTargetEntry& entry) const;
	float GetFollowTargetRadius(const FollowTargetEntry& entry) const;
	glm::vec3 ComputeFollowCameraPosition(const FollowTargetEntry& entry, const glm::vec3& targetPos) const;
	glm::mat4 BuildMoonModelMatrix() const;
private:
	Window window;	//Window must be constructed first, it initializes OpenGL context and GLFW window.
private:
	ShaderProgram defaultShader;
	ShaderProgram noLightShader;
	ShaderProgram earthShader;
	ShaderProgram overlayShader; // translucent overlays (atmospheres, rings)
	ShaderProgram textShader;
	ShaderProgram orbitShader;
	Camera camera;
	Mesh sphereMesh;            // shared sphere geometry
	Mesh ringMesh;              // unit ring in XZ plane; scaled per planet
	std::vector<Texture> planetTextures; // sun + planet albedo maps
	std::vector<Planet> planets;         // orbital/self-rotation state
	Texture earthNightTexture;
	Texture earthSpecularTexture;
	Texture earthCloudsTexture;
	Texture venusAtmosphereTexture;
	Texture saturnRingTexture;
	Texture moonTexture;
	Texture uranusRingTexture;
	Actor skyBox;
	Texture skyboxTexture;
	Actor venusAtmosphere;
	Actor saturnRings;
	Actor uranusRings;
	Actor moon;
	float moonOrbitDeg = 0.0f;
	glm::vec2 lastMousePosition;
	float lastTime;
	float shaderTime = 0.0f; // accumulates only when not paused for stable pause state
	float timeSpeed = 1.0f;  // time dilation factor
	bool isPaused = false;   // pause flag; freezes celestial motion but camera still moves
	double simulationTimeSec = 0.0; // advances when not paused
	// Asteroid belt
	struct Asteroid { float radius; float angleDeg; float angularSpeedDeg; glm::vec3 scale; glm::vec3 offset; float spinDeg; glm::vec3 spinAxis; float spinSpeedDeg; int meshIndex; };
	std::vector<Asteroid> asteroids;
	std::vector<Mesh> asteroidMeshes; // small pool of irregular rock meshes
	Texture asteroidTexture;
	unsigned int textVAO = 0;
	unsigned int textVBO = 0;
	std::vector<float> hudVertices;
	glm::mat4 hudProjection = glm::mat4(1.0f);
	struct PlanetHudInfo
	{
		std::string name;
		float orbitRadiusMillionKm;
		float orbitSpeedKmPerSec;
		float rotationPeriodHours;
		bool retrograde;
	};
	std::vector<PlanetHudInfo> planetHudData;
	PlanetHudInfo moonHudData;
	std::string followHudText;
	struct OrbitPath
	{
		unsigned int vao = 0;
		unsigned int vbo = 0;
		GLsizei vertexCount = 0;
		float radius = 0.0f;
		glm::vec3 color = glm::vec3(0.6f);
	};
	std::vector<OrbitPath> orbitPaths;
	OrbitPath moonOrbitPath;
	bool showOrbitPaths = false;
	// Kepler elements per planet index (matching textures/planets vector indices)
	std::vector<OrbitalElements> planetElements; // size planets.size(); [1..8] valid
	std::vector<float> planetSelfRotationDeg;     // accumulate self rotation per planet (1..8)
	std::vector<float> planetRotationSpeedDeg;    // rotation speed per planet (deg/sec equivalent of legacy)
	std::vector<float> planetObliquityDeg;        // axial tilt per planet
	// N-body Moon state (Earth + optional Sun acceleration)
	double lastSimTimeSec = 0.0;
	float muEarth = 0.0f;
	float muSun = 0.0f;
	glm::vec3 moonRelPos = glm::vec3(0.0f); // Moon position relative to Earth
	glm::vec3 moonRelVel = glm::vec3(0.0f); // Moon velocity relative to Earth
	struct FollowTargetEntry
	{
		enum class Kind { Planet, Moon } kind;
		size_t index;
	};
	std::vector<FollowTargetEntry> followTargets;
	int followTargetIndex = -1;
	glm::vec3 savedCameraPosition;
	float savedCameraYaw = 0.0f;
	float savedCameraPitch = 0.0f;
	bool hasSavedCameraPose = false;
	glm::mat4 moonModelMatrix = glm::mat4(1.0f);
};