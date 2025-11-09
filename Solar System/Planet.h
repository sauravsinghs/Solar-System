#pragma once
#include "Actor.h"

// A scene actor with simple orbital mechanics around the origin (sun).
// Maintains current orbit and self-rotation angles; builds model matrix each update.
class Planet : public Actor
{
public:
	/// <param name="orbitRadius">The radius of the orbit around the center of the world.</param>
	/// <param name="scale">The scale of the planet.</param>
	/// <param name="orbitSpeedDegrees">The number of degrees the planet should travel around its orbit every second.</param>
	/// <param name="rotationSpeedDegrees">The number of degrees the planet should rotate around itself every second.</param>
	Planet(float orbitRadius, float scale, float orbitSpeedDegrees, float rotationSpeedDegrees);
    void Update(float deltatime);
    // For Kepler-driven planets, Game sets the model directly; expose a helper to set self-rotation only
    void SetSelfRotationDegrees(float degrees) { currentRotationDegrees = degrees; }
    float GetScale() const { return scale; }
public:
	float GetOrbitDegrees() const;        // Current orbit angle in degrees
	float GetRotationDegrees() const;     // Current self-rotation angle in degrees
    float GetOrbitRadius() const { return orbitRadius; }
private:
	const float orbitRadius;
	const float scale;
	const float orbitSpeedDegrees;
	const float rotationSpeedDegrees;
	float currentOrbitDegrees = 0.f;
	float currentRotationDegrees = 0.f;
};
