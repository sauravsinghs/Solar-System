#include "Actor.h"
#include <glm/gtc/matrix_transform.hpp>

void Actor::ResetModelMatrix()
{
	modelMatrix = glm::mat4(1.0f);
}
// → modelMatrix ko identity matrix set karta hai (means no transform).
void Actor::ApplyTranslation(glm::vec3 translation)
{
	modelMatrix = glm::translate(modelMatrix, translation);
}
// Object ko move karta hai.

// Agar translation = (0, 1, 0) → object 1 unit upar move karega.

void Actor::ApplyScale(glm::vec3 scale)
{
	modelMatrix = glm::scale(modelMatrix, scale);
}

// Object ka size change karta hai.
// Agar scale = (2, 2, 2) → object ka size double
void Actor::ApplyRotation(float degrees, glm::vec3 axis)
{
	modelMatrix = glm::rotate(modelMatrix, glm::radians(degrees), axis);
}
// Object ko rotate karta hai.axis ke around.

const glm::mat4 &Actor::GetModelMatrix() const
{
	return modelMatrix;
}
// Model matrix ko return karta hai.

glm::mat3 Actor::GetNormalMatrix() const
{
	return glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
}

// → Lighting calculation ke liye surface normals ko correct karne ke liye use hota hai.
void Actor::SetModelMatrix(const glm::mat4 &model)
{
	modelMatrix = model;
}

// Model matrix ko directly set karta hai.
