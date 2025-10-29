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
    lastMousePosition = window.GetMousePosition();
    lastTime = window.GetElapsedTime();
    skyBox.ApplyScale(glm::vec3{ settings::cameraFarPlaneDistance });
    //Load the textures and initialize the planets.
    // Include Sun, Earth, Mars only
    planetTextures.emplace_back(settings::texturesPath + "sun.jpg");
    planets.emplace_back(0.f, settings::sunScale, 0.f, settings::sunRotationSpeed);
    planetTextures.emplace_back(settings::texturesPath + "earth.jpg");
    planets.emplace_back(settings::earthOrbitRadius, settings::earthScale, settings::earthOrbitSpeed, settings::earthRotationSpeed);
    planetTextures.emplace_back(settings::texturesPath + "mars.jpg");
    planets.emplace_back(settings::marsOrbitRadius, settings::marsScale, settings::marsOrbitSpeed, settings::marsRotationSpeed);
    //Setup the lighting in the shaders.
    window.UseShader(defaultShader);
    defaultShader.SendUniform<glm::vec3>("viewPosition", camera.GetPosition());
    
    defaultShader.SendUniform<glm::vec3>("lightPosition", { 0.0f,0.0f,0.0f });
    defaultShader.SendUniform<glm::vec3>("ambientColor", settings::ambientColor);
    defaultShader.SendUniform<glm::vec3>("sunlightColor", settings::sunlightColor);
    // no advanced earth shader in minimal build
    // no overlay shader in minimal build

    // no ring mesh in minimal build

    // no asteroid belt in minimal build
}

void Game::Tick()
{
    // ek full frame cycle 
    //Measure the time that has passed since the previous frame.
    float now = window.GetElapsedTime(); // current time calcutate kiya
    float deltatime = now - lastTime;  // time difference between current and last frame
    lastTime = now;

    window.ClearBuffers();  //Clears the color and depth buffers.
    Update(deltatime);  // purane cheezo ko update karta hai 
    Draw(deltatime);     // rendering karta hai
    window.SwapBuffers();	//Swap the current buffer to display it.
    window.PollEvents();    //Process the pending window events.
}

bool Game::ShouldClose() const
{
    return window.ShouldClose();
}

void Game::Update(float deltatime)
{
    //Logic update happens here.
    //Check if window should be closed.
    if(window.IsKeyPressed(settings::exitKey))
    {
        // esc key window close kar deta hai
        window.Close();
    }
    //Update camera rotation.
    // mouse movement ke according camera ko rotate karta hai
    glm::vec2 mousePosition = window.GetMousePosition();
    glm::vec2 cameraRotationOffset{mousePosition.x - lastMousePosition.x, lastMousePosition.y - mousePosition.y };
    lastMousePosition = mousePosition;
    camera.Rotate(cameraRotationOffset);
    //Update camera position.
    // wasd + space /ctrl ke through camera move karta hai
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
    //Update wireframe mode.
    if (window.IsKeyPressedOnce(settings::wireframeModeKey))
    {
        window.ToggleWireframe();
    }
    //Update camera speed.
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
        // each planet update its own orbits and rotations
        // using its own update method
        // Advance shader animation time only when running
        shaderTime += deltatime;
        //Update the planets' transforms.
        for (Planet& planet : planets)
        {
            planet.Update(deltatime * timeSpeed);
        }
        // no moon or asteroids in minimal build

        //Update the time speed.
        if (window.IsKeyPressed(settings::timeSpeedupKey))
            timeSpeed += settings::timeAdjustSpeed;
        if (window.IsKeyPressed(settings::timeSlowdownKey))
            timeSpeed -= settings::timeAdjustSpeed;
    }
}

void Game::Draw(float deltatime)
{
    //Drawing happens here.
    glm::mat4 projection = camera.GetPerspectiveMatrix();
    glm::mat4 viewMatrix = camera.GetViewMatrix();
    //Draw the planets (except for the sun and the earth) using the default shader (normal lighting).
    window.UseShader(defaultShader);
    defaultShader.SendUniform<glm::vec3>("viewPosition", camera.GetPosition());
    // no animated uniforms in minimal build
    for (size_t i = 2; i < planets.size(); ++i)
    {
        // no flow uniform in minimal build
        // this is for mars texture animation
        glm::mat4 model = planets[i].GetModelMatrix();  // it get model matrix from actor 
        defaultShader.SendUniform<glm::mat4>("MVP", projection * viewMatrix * model);  // object model ke baad world  mein mvp banta hai 
        defaultShader.SendUniform<glm::mat4>("modelMatrix", model);
        defaultShader.SendUniform<glm::mat3>("normalMatrix", planets[i].GetNormalMatrix());
        window.DrawActor(sphereMesh, planetTextures[i]);
    }
    //Draw the earth using basic shader
    window.UseShader(defaultShader);
    defaultShader.SendUniform<glm::vec3>("viewPosition", camera.GetPosition());
    
    
    {
        glm::mat4 model = planets[1].GetModelMatrix();
        defaultShader.SendUniform<glm::mat4>("MVP", projection * viewMatrix * model);
        defaultShader.SendUniform<glm::mat4>("modelMatrix", model);
        defaultShader.SendUniform<glm::mat3>("normalMatrix", planets[1].GetNormalMatrix());
        window.DrawActor(sphereMesh, planetTextures[1]);
    }

    // no Venus atmosphere in minimal build

    // Saturn rings overlay using ring mesh and overlay shader
    // no Saturn rings

    // Uranus rings (thin, smaller, vertical alignment) using same ring mesh
    // no Uranus rings

    // Asteroid belt draw (after planets, before sun/skybox)
    // no asteroid belt draw

    // Moon: orbit Earth and stay tidally locked (same face toward Earth)
    // no moon
    // (cleanup) removed stray placeholder Venus code
    //Draw the sun and the skybox without lighting.
    window.UseShader(noLightShader);
    //Draw sun with no-light shader but animated UVs and brightness boost
    window.UseShader(noLightShader);
    noLightShader.SendUniform<glm::mat4>("MVP", projection * viewMatrix * planets[0].GetModelMatrix());
    
    window.DrawActor(sphereMesh, planetTextures[0]);
    //Draw skybox.
    viewMatrix = glm::mat4(glm::mat3(viewMatrix));//Remove the translation from the view matrix, we do not want our skybox to move around.
    
    noLightShader.SendUniform<glm::mat4>("MVP", projection * viewMatrix * skyBox.GetModelMatrix());
    window.DrawActor(sphereMesh, skyboxTexture);
}
