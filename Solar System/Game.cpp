// Game: owns runtime resources, updates simulation, and renders each frame.
#include "Game.h"
#include "Settings.h"
#include <glm/gtc/matrix_transform.hpp>
#include <random>

Game::Game(int windowWidth, int windowHeight, int viewportX, int viewportY, int viewportWidth, int viewportHeight, const std::string title, GLFWmonitor* monitor, GLFWwindow* share)
    :
    window(windowWidth, windowHeight, viewportX, viewportY, viewportWidth, viewportHeight, title, monitor, share),
    defaultShader(settings::shadersPath + "default.vert", settings::shadersPath + "default.frag"),
    noLightShader(settings::shadersPath + "noLight.vert", settings::shadersPath + "noLight.frag"),
    camera(settings::cameraInitialPosition, settings::cameraSpeed, settings::cameraYaw,
        settings::cameraPitch, settings::cameraMaxPitch, settings::cameraSensitivity, settings::cameraFOV,
        settings::screenRatio, settings::cameraNearPlaneDistance, settings::cameraFarPlaneDistance),
    sphereMesh(settings::meshesPath + "sphere.obj"),
    skyboxTexture(settings::texturesPath + "stars_milkyway.jpg")
    
{
    // Initialize per-frame state and giant skybox sphere.
    lastMousePosition = window.GetMousePosition();
    lastTime = window.GetElapsedTime();
    skyBox.ApplyScale(glm::vec3{ settings::cameraFarPlaneDistance });
    // Load textures and set up planets (minimal: Sun, Earth, Mars).
    planetTextures.emplace_back(settings::texturesPath + "sun.jpg");
    planets.emplace_back(0.f, settings::sunScale, 0.f, settings::sunRotationSpeed);
    planetTextures.emplace_back(settings::texturesPath + "earth.jpg");
    planets.emplace_back(settings::earthOrbitRadius, settings::earthScale, settings::earthOrbitSpeed, settings::earthRotationSpeed);
    planetTextures.emplace_back(settings::texturesPath + "mars.jpg");
    planets.emplace_back(settings::marsOrbitRadius, settings::marsScale, settings::marsOrbitSpeed, settings::marsRotationSpeed);
    // Set up lighting uniforms once; updated viewPosition per-frame.
    window.UseShader(defaultShader);
    defaultShader.SendUniform<glm::vec3>("viewPosition", camera.GetPosition());
    
    defaultShader.SendUniform<glm::vec3>("lightPosition", { 0.0f,0.0f,0.0f });
    defaultShader.SendUniform<glm::vec3>("ambientColor", settings::ambientColor);
    defaultShader.SendUniform<glm::vec3>("sunlightColor", settings::sunlightColor);
}

void Game::Tick()
{
    //Measure the time that has passed since the previous frame.
    float now = window.GetElapsedTime();
    float deltatime = now - lastTime;
    lastTime = now;

    // Clear buffers, update logic, draw, then present and process events.
    window.ClearBuffers();  // Clears the color and depth buffers.
    Update(deltatime);
    Draw(deltatime);
    window.SwapBuffers();	//Swap the current buffer to display it.
    window.PollEvents();    //Process the pending window events.
}

bool Game::ShouldClose() const
{
    return window.ShouldClose();
}

void Game::Update(float deltatime)
{
    // Handle input and update simulation state.
    // Check if window should be closed.
    if(window.IsKeyPressed(settings::exitKey))
    {
        window.Close();
    }
    // Update camera rotation from mouse delta.
    glm::vec2 mousePosition = window.GetMousePosition();
    glm::vec2 cameraRotationOffset{mousePosition.x - lastMousePosition.x, lastMousePosition.y - mousePosition.y };
    lastMousePosition = mousePosition;
    camera.Rotate(cameraRotationOffset);
    // Update camera position from WASD/QE.
    if (window.IsKeyPressed(settings::forwardKey))
        camera.Move(Camera::Movement::FORWARD, deltatime);
    if (window.IsKeyPressed(settings::backwardKey))
        camera.Move(Camera::Movement::BACKWARD, deltatime);
    if (window.IsKeyPressed(settings::leftKey))
        camera.Move(Camera::Movement::LEFT, deltatime);
    if (window.IsKeyPressed(settings::rightKey))
        camera.Move(Camera::Movement::RIGHT, deltatime);
    if (window.IsKeyPressed(settings::upKey1) || window.IsKeyPressed(settings::upKey2))
        camera.Move(Camera::Movement::UP, deltatime);
    if (window.IsKeyPressed(settings::downKey))
        camera.Move(Camera::Movement::DOWN, deltatime);
    // Toggle wireframe mode.
    if (window.IsKeyPressedOnce(settings::wireframeModeKey))
    {
        window.ToggleWireframe();
    }
    // Adjust camera movement speed.
    if (window.IsKeyPressed(settings::cameraSpeedupKey))
    {
        camera.AddMovementSpeed(settings::cameraSpeedupRate);
    }
    if (window.IsKeyPressed(settings::cameraSlowdownKey))
    {
        camera.AddMovementSpeed(-settings::cameraSpeedupRate);
    }
    if (window.IsKeyPressedOnce(settings::pauseKey))
    {
        isPaused = !isPaused;
    }

    if (!isPaused)
    {
        // Update the planets' transforms.
        for (Planet& planet : planets)
        {
            planet.Update(deltatime * timeSpeed);
        }

        // Update the time speed.
        if (window.IsKeyPressed(settings::timeSpeedupKey))
            timeSpeed += settings::timeAdjustSpeed;
        if (window.IsKeyPressed(settings::timeSlowdownKey))
            timeSpeed -= settings::timeAdjustSpeed;
    }
}

void Game::Draw(float deltatime)
{
    // Build camera matrices and draw scene objects.
    glm::mat4 projection = camera.GetPerspectiveMatrix();
    glm::mat4 viewMatrix = camera.GetViewMatrix();
    // Draw the planets (except for the sun and the earth) using the default shader (normal lighting).
    window.UseShader(defaultShader);
    defaultShader.SendUniform<glm::vec3>("viewPosition", camera.GetPosition());
    
    for (size_t i = 2; i < planets.size(); ++i)
    {
        
        glm::mat4 model = planets[i].GetModelMatrix();
        defaultShader.SendUniform<glm::mat4>("MVP", projection * viewMatrix * model);
        defaultShader.SendUniform<glm::mat4>("modelMatrix", model);
        defaultShader.SendUniform<glm::mat3>("normalMatrix", planets[i].GetNormalMatrix());
        window.DrawActor(sphereMesh, planetTextures[i]);
    }
    // Draw the earth using basic shader.
    window.UseShader(defaultShader);
    defaultShader.SendUniform<glm::vec3>("viewPosition", camera.GetPosition());
    
    
    {
        glm::mat4 model = planets[1].GetModelMatrix();
        defaultShader.SendUniform<glm::mat4>("MVP", projection * viewMatrix * model);
        defaultShader.SendUniform<glm::mat4>("modelMatrix", model);
        defaultShader.SendUniform<glm::mat3>("normalMatrix", planets[1].GetNormalMatrix());
        window.DrawActor(sphereMesh, planetTextures[1]);
    }

    window.UseShader(noLightShader);
    // Draw sun (unlit/emissive) with slight brightness boost in fragment shader.
    window.UseShader(noLightShader);
    noLightShader.SendUniform<glm::mat4>("MVP", projection * viewMatrix * planets[0].GetModelMatrix());
    
    window.DrawActor(sphereMesh, planetTextures[0]);
    // Draw skybox: remove camera translation so skybox appears infinitely far.
    viewMatrix = glm::mat4(glm::mat3(viewMatrix));
    
    noLightShader.SendUniform<glm::mat4>("MVP", projection * viewMatrix * skyBox.GetModelMatrix());
    window.DrawActor(sphereMesh, skyboxTexture);
}
