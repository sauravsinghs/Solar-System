#pragma once   // prevent multiple inclusions of this header file
#include <glm/glm.hpp>

// A camera class that handles and calculates both the projection and view matrices.
class Camera
{
public:
    static constexpr glm::vec3 worldUp = { 0.0f, 1.0f, 0.0f };
    // worldUp → ek constant direction vector jo batata hai ki world me “upar” direction kaunsi hai (y-axis up).
     // static constexpr → ye har object ke liye common hai, memory me sirf ek hi instance hota hai.
    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods.
    enum class Movement
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };
    // ye enum keyboard controls ke liye input provide karta hai jaise WASD controls.
public:
    Camera(glm::vec3 position, float movementSpeed, float yaw, float pitch, float maxPitch,
        float mouseSensitivity, float zoom, float screenRatio, float nearPlaneDistance = 0.1f, float farPlaneDistance = 100.f); // yaw /pitch -> camera kis direction mein dikh raha hai 
        // screenRatio -> aspect ratio of the viewport width/height   
    glm::mat4 GetViewMatrix() const;        //Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetPerspectiveMatrix() const; //Returns the perspective projection matrix using the zoom and screen ratio.
    void Move(Movement direction, float deltaTime); // keyboard ke through camera ko move karta hai
    void Rotate(glm::vec2 rotationOffset, bool constrainPitch = true);// mouse ke according camera ko rotate karta hai
    void AddMovementSpeed(float speed);    // camera ki movement speed ko adjust karta hai aur ensure karta hai ki speed negative na ho.
    const glm::vec3& GetPosition() const; // return current position of the camera
private:
    void UpdateCameraVectors(); //Updates the front, right and up vectors from the Camera's (updated) jab bhi yaw ya pitch change hota hai tab is function ko call karna padta hai taaki camera ki direction sahi rahe.
private:
    // camera Attributes
    glm::vec3 position;
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up;
    glm::vec3 right;
    // euler Angles
    float yaw;
    float pitch;
    float maxPitch;
    // camera options
    float movementSpeed;
    float mouseSensitivity;
    float zoom;
    float screenRatio;
    float nearPlaneDistance;
    float farPlaneDistance;
};

