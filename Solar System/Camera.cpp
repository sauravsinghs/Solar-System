#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp> // it include function like lookAt, Perspective and translate 
#include <algorithm>

// Define static constexpr member for older compilers/linkers
constexpr glm::vec3 Camera::worldUp;

Camera::Camera(glm::vec3 position, float movementSpeed, float yaw, float pitch, float maxPitch,
               float mouseSensitivity, float zoom, float screenRatio, float nearPlaneDistance, float farPlaneDistance)
    : position(position),
      yaw(yaw),
      pitch(pitch),
      maxPitch(maxPitch),
      movementSpeed(movementSpeed),
      mouseSensitivity(mouseSensitivity),
      zoom(zoom),
      screenRatio(screenRatio),
      nearPlaneDistance(nearPlaneDistance),
      farPlaneDistance(farPlaneDistance)
      // Sab values initialize karta hai, aur baad me
      // UpdateCameraVectors() call karta hai
      // taaki front/right/up correct set ho jaaye.
{
    UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(position, position + front, up);
}
// glm::lookAt() → ek matrix banata
// hai jo camera ke world position se scene 
//ko “dekhne” ke liye use hoti hai.

// Example: Agar camera (0,0,3) pe hai aur front (0,0,-1) hai
//→ wo origin (0,0,0) ko dekh raha hai.

glm::mat4 Camera::GetPerspectiveMatrix() const
{
    return glm::perspective(glm::radians(zoom), screenRatio, nearPlaneDistance, farPlaneDistance);
}
// Yeh matrix objects ko perspective distortion deta hai (near objects bade, far chhote).
//Shader me “P” (Projection Matrix) ke liye use hoti hai.

void Camera::Move(Movement direction, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;
    if (direction == Movement::FORWARD)
        position += front * velocity;
    if (direction == Movement::BACKWARD)
        position -= front * velocity;
    if (direction == Movement::LEFT)
        position -= right * velocity;
    if (direction == Movement::RIGHT)
        position += right * velocity;
    if (direction == Movement::UP)
        position += worldUp * velocity;
    if (direction == Movement::DOWN)
        position -= worldUp * velocity;
}
// Camera ko move karta hai forward/backward/left/right/up/down depending on input.
//deltaTime ensure karta hai ki movement frame rate independent ho.

void Camera::Rotate(glm::vec2 rotationOffset, bool constrainPitch)
{
    rotationOffset *= mouseSensitivity;
    yaw += rotationOffset.x;
    pitch += rotationOffset.y;
    // Constrain the pitch so that the screen doesn't get flipped.
    if (constrainPitch)
    {
        if (pitch > maxPitch)
            pitch = maxPitch;
        if (pitch < -maxPitch)
            pitch = -maxPitch;
    }
    // Mouse ke movement ke hisaab se camera rotate hota hai (yaw → left/right, pitch → up/down).
    // Pitch ko clamp kiya jaata hai taaki camera ulta na ho jaaye.
    // Update front, right and up vectors using the updated Euler angles.
    UpdateCameraVectors();
}

void Camera::AddMovementSpeed(float speed)
{
    movementSpeed = std::max(movementSpeed + speed, 0.0f);
}

const glm::vec3 &Camera::GetPosition() const
{
    return position;
}

void Camera::UpdateCameraVectors()
{
    // Re-calculate the front vector.
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    // Re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(front, worldUp)); // normalize the vectors, because their length gets closer to 0 the more we look up or down which results in slower movement.
    up = glm::normalize(glm::cross(right, front));
}

// Yaw & Pitch se naya front vector calculate hota hai
// (mathematical direction camera jidhhar dekh raha hai).
//Uske cross products se right aur up vector nikalte hain 
//— jisse camera ka local coordinate system ban jaata hai.