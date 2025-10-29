#pragma once
#include <glm/glm.hpp>
//A class to represent an entity in the world, handling its transformations.
class Actor
{
public:
	void ResetModelMatrix();	 // sab kuch reset ho jata hai							
	void ApplyTranslation(glm::vec3 translation);	 // move kare object		//Applies a translation on the model matrix.
	void ApplyScale(glm::vec3 scale);	 // size change ho					//Applies a scale on the model matrix.
	void ApplyRotation(float degrees, glm::vec3 axis); 		//Applies a rotation on the model matrix.
	void SetModelMatrix(const glm::mat4& model); // matrix ko overwrite krta hai			//Sets model matrix directly (for hierarchical transforms).
	const glm::mat4& GetModelMatrix() const;  // acess the matrix 
	glm::mat3 GetNormalMatrix() const;
private:
	//The model matrix that represents the position, orientation and scale of the actor.
	glm::mat4 modelMatrix = glm::mat4(1.0f);
};