#pragma once
#include <glm/glm.hpp>
// Lightweight transform holder for scene objects. Provides model and derived normal matrices
// and helpers to compose translation/rotation/scale in a clear order.
class Actor
{
public:
	void ResetModelMatrix();								//Resets the model matrix to the identity matrix.
	void ApplyTranslation(glm::vec3 translation);			//Applies a translation on the model matrix.
	void ApplyScale(glm::vec3 scale);						//Applies a scale on the model matrix.
	void ApplyRotation(float degrees, glm::vec3 axis);		//Applies a rotation on the model matrix.
	void SetModelMatrix(const glm::mat4& model);			//Sets model matrix directly (for hierarchical transforms).
	const glm::mat4& GetModelMatrix() const;
	glm::mat3 GetNormalMatrix() const;
private:
	//The model matrix that represents the position, orientation and scale of the actor.
	glm::mat4 modelMatrix = glm::mat4(1.0f);
};