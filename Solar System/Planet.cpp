#include "Planet.h"

Planet::Planet(float orbitRadius, float scale, float orbitSpeedDegrees, float rotationSpeedDegrees)
	:
	orbitRadius(orbitRadius),
	scale(scale),
	orbitSpeedDegrees(orbitSpeedDegrees),
	rotationSpeedDegrees(rotationSpeedDegrees)
{
}

// Yeh sirf values initialize karta hai (planet ki basic properties).
// Jaise:

// orbitRadius = kitni doori par hai sun se

// scale = planet ka size (Earth chhota, Jupiter bada)

// orbitSpeedDegrees = sun ke around ghoomne ki speed

// rotationSpeedDegrees = apne axis par ghoomne ki speed

void Planet::Update(float deltatime)
{
	currentRotationDegrees += rotationSpeedDegrees * deltatime;
	currentOrbitDegrees += orbitSpeedDegrees * deltatime;
	// Step 2️⃣ — Angles ko 360° ke andar rakhna
	if (currentRotationDegrees > 360.0f)
		currentRotationDegrees = currentRotationDegrees - 360.0f;
	else if (currentRotationDegrees < -360.0f)
		currentRotationDegrees = currentRotationDegrees + 360.0f;

	if (currentOrbitDegrees > 360.0f)
		currentOrbitDegrees = currentOrbitDegrees - 360.0f;
	else if (currentOrbitDegrees < -360.0f)
		currentOrbitDegrees = currentOrbitDegrees + 360.0f;
	//Calculate planet transforms.
	ResetModelMatrix();  // clears previous transforms
	ApplyRotation(currentOrbitDegrees, { 0.f,1.f,0.f });// Rotates around the sun.
	ApplyTranslation({ orbitRadius, 0.0f, 0.0f }); // sun se distance calculate karta hai
	ApplyScale(glm::vec3{ scale });  // planet ka size set karta hai
	ApplyRotation(currentRotationDegrees, {0.f,1.f,0.f}); //Revolves around itself.
}

float Planet::GetOrbitDegrees() const
{
    return currentOrbitDegrees;
}

float Planet::GetRotationDegrees() const
{
    return currentRotationDegrees;
}