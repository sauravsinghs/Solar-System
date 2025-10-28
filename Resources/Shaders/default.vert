// Default lit vertex shader: passes UV, world-space position, and transformed normal.
#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexture;
layout (location = 2) in vec3 aNormal;

out vec2 textureCoordinate;
out vec3 normal;
out vec3 fragmentPosition;

uniform mat4 MVP;          // projection * view * model
uniform mat4 modelMatrix;  // model matrix for world position
uniform mat3 normalMatrix; // inverse-transpose of upper-left of model

void main()
{
    textureCoordinate = vec2(aTexture.x, 1.0 - aTexture.y);
    normal = normalMatrix * aNormal;
    fragmentPosition = vec3(modelMatrix * vec4(aPosition, 1.0));
    gl_Position = MVP * vec4(aPosition, 1.0);
}