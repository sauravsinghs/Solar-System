// FPS-style camera with yaw/pitch control, movement along local axes, and perspective setup.
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>
#include <algorithm>
#include <limits>

// Define static constexpr member for older compilers/linkers
constexpr glm::vec3 Camera::worldUp;

Camera::Camera(glm::vec3 position, float movementSpeed, float yaw, float pitch, float maxPitch,
    float mouseSensitivity, float zoom, float screenRatio, float nearPlaneDistance, float farPlaneDistance)
    :
    position(position),
    yaw(yaw),
    pitch(pitch),
    maxPitch(maxPitch),
    movementSpeed(movementSpeed),
    mouseSensitivity(mouseSensitivity),
    zoom(zoom),
    screenRatio(screenRatio),
    nearPlaneDistance(nearPlaneDistance),
    farPlaneDistance(farPlaneDistance)
{
    UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::GetPerspectiveMatrix() const
{
    return glm::perspective(glm::radians(zoom), screenRatio, nearPlaneDistance, farPlaneDistance);
}

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

void Camera::Rotate(glm::vec2 rotationOffset, bool constrainPitch)
{
    rotationOffset *= mouseSensitivity;
    yaw += rotationOffset.x;
    pitch += rotationOffset.y;
    //Constrain the pitch so that the screen doesn't get flipped.
    if (constrainPitch)
    {
        if (pitch > maxPitch)
            pitch = maxPitch;
        if (pitch < -maxPitch)
            pitch = -maxPitch;
    }
    //Update front, right and up vectors using the updated Euler angles.
    UpdateCameraVectors();
}

void Camera::AddMovementSpeed(float speed)
{
    movementSpeed = std::max(movementSpeed + speed, 0.0f);
}

const glm::vec3& Camera::GetPosition() const
{
    return position;
}


const glm::vec3& Camera::GetFront() const
{
    return front;
}

float Camera::GetYaw() const
{
    return yaw;
}

float Camera::GetPitch() const
{
    return pitch;
}

void Camera::SetPose(const glm::vec3& newPosition, float newYaw, float newPitch)
{
    position = newPosition;
    yaw = newYaw;
    pitch = glm::clamp(newPitch, -maxPitch, maxPitch);
    UpdateCameraVectors();
}

void Camera::LookAt(const glm::vec3& eye, const glm::vec3& target)
{
    glm::vec3 toTarget = target - eye;
    if (glm::length(toTarget) <= std::numeric_limits<float>::epsilon())
    {
        position = eye;
        return;
    }

    glm::vec3 direction = glm::normalize(toTarget);
    float newPitch = glm::degrees(asin(glm::clamp(direction.y, -1.0f, 1.0f)));
    float newYaw = glm::degrees(atan2(direction.z, direction.x));
    SetPose(eye, newYaw, newPitch);
}


void Camera::UpdateCameraVectors()
{
    //Re-calculate the front vector.
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    //Re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(front, worldUp));//normalize the vectors, because their length gets closer to 0 the more we look up or down which results in slower movement.
    up = glm::normalize(glm::cross(right, front));
}
