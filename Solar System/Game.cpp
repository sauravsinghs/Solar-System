// Implements the simulation step and rendering of the solar system.
// Responsibilities:
//  - Initialize shaders, meshes (sphere, ring), textures, and planet parameters
//  - Handle input (camera motion, toggles, time scaling)
//  - Advance or pause simulation state (planets, moon, asteroids)
//  - Render solid bodies (default + earth shader), overlays (atmospheres/rings), and unlit (sun, skybox)
#include "Game.h"
#include "Settings.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glad/glad.h>
#include <random>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include "../Dependencies/stb_easy_font.h"
#include "Orbital.h"

Game::Game(int windowWidth, int windowHeight, int viewportX, int viewportY, int viewportWidth, int viewportHeight, const std::string title, GLFWmonitor* monitor, GLFWwindow* share)
    :
    window(windowWidth, windowHeight, viewportX, viewportY, viewportWidth, viewportHeight, title, monitor, share),
    defaultShader(settings::shadersPath + "default.vert", settings::shadersPath + "default.frag"),
    noLightShader(settings::shadersPath + "noLight.vert", settings::shadersPath + "noLight.frag"),
    overlayShader(settings::shadersPath + "overlay.vert", settings::shadersPath + "overlay.frag"),
    textShader(settings::shadersPath + "text.vert", settings::shadersPath + "text.frag"),
    orbitShader(settings::shadersPath + "orbit.vert", settings::shadersPath + "orbit.frag"),
    earthShader(settings::shadersPath + "earth.vert", settings::shadersPath + "earth.frag"),
    camera(settings::cameraInitialPosition, settings::cameraSpeed, settings::cameraYaw,
        settings::cameraPitch, settings::cameraMaxPitch, settings::cameraSensitivity, settings::cameraFOV,
        settings::screenRatio, settings::cameraNearPlaneDistance, settings::cameraFarPlaneDistance),
    sphereMesh(settings::meshesPath + "sphere.obj"),
    earthNightTexture(settings::texturesPath + "earth_night.jpg"),
    earthSpecularTexture(settings::texturesPath + "earth_specular.jpg"),
    earthCloudsTexture(settings::texturesPath + "earth_clouds.jpg"),
    skyboxTexture(settings::texturesPath + "stars_milkyway.jpg"),
    venusAtmosphereTexture(settings::texturesPath + "venus atmosphere.jpg"),
    saturnRingTexture(settings::texturesPath + "saturn ring.png"),
    moonTexture(settings::texturesPath + "moon.jpg"),
    asteroidTexture(settings::texturesPath + "asteroid.jpg"),
    uranusRingTexture(settings::texturesPath + "saturn ring.png")
{
    lastMousePosition = window.GetMousePosition();
    lastTime = window.GetElapsedTime();
    skyBox.ApplyScale(glm::vec3{ settings::cameraFarPlaneDistance });
    //Load the textures and initialize the planets.
    //Sun
    planetTextures.emplace_back(settings::texturesPath + "sun.jpg");
    planets.emplace_back(0.f, settings::sunScale, 0.f, settings::sunRotationSpeed);
    //Earth
    planetTextures.emplace_back(settings::texturesPath + "earth.jpg");
    planets.emplace_back(settings::earthOrbitRadius, settings::earthScale, settings::earthOrbitSpeed, settings::earthRotationSpeed);
    //Mercury
    planetTextures.emplace_back(settings::texturesPath + "mercury.jpg");
    planets.emplace_back(settings::mercuryOrbitRadius, settings::mercuryScale, settings::mercuryOrbitSpeed, settings::mercuryRotationSpeed);
    //Venus
    planetTextures.emplace_back(settings::texturesPath + "venus.jpg");
    planets.emplace_back(settings::venusOrbitRadius, settings::venusScale, settings::venusOrbitSpeed, settings::venusRotationSpeed);
    //Mars
    planetTextures.emplace_back(settings::texturesPath + "mars.jpg");
    planets.emplace_back(settings::marsOrbitRadius, settings::marsScale, settings::marsOrbitSpeed, settings::marsRotationSpeed);
    //Jupiter
    planetTextures.emplace_back(settings::texturesPath + "jupiter.jpg");
    planets.emplace_back(settings::jupiterOrbitRadius, settings::jupiterScale, settings::jupiterOrbitSpeed, settings::jupiterRotationSpeed);
    //Saturn
    planetTextures.emplace_back(settings::texturesPath + "saturn.jpg");
    planets.emplace_back(settings::saturnOrbitRadius, settings::saturnScale, settings::saturnOrbitSpeed, settings::saturnRotationSpeed);
    //Uranus
    planetTextures.emplace_back(settings::texturesPath + "uranus.jpg");
    planets.emplace_back(settings::uranusOrbitRadius, settings::uranusScale, settings::uranusOrbitSpeed, settings::uranusRotationSpeed);
    //Neptune
    planetTextures.emplace_back(settings::texturesPath + "neptune.jpg");
    planets.emplace_back(settings::neptuneOrbitRadius, settings::neptuneScale, settings::neptuneOrbitSpeed, settings::neptuneRotationSpeed);

    // Kepler elements (approx J2000) scaled to scene units (a in Earth-orbit-radius units)
    planetElements.resize(planets.size());
    planetSelfRotationDeg.assign(planets.size(), 0.0f);
    planetRotationSpeedDeg.clear();
    planetObliquityDeg.clear();
    planetRotationSpeedDeg.push_back(settings::sunRotationSpeed);
    planetObliquityDeg.push_back(0.0f); // Sun (unused)

    auto AU = settings::keplerAUScale; // scene units per AU for Kepler mapping
    auto make = [&](int idx, double aAU, double e, double i, double Om, double om, double M0, double nDegPerDay,
                    float rotSpeedDeg, float obliqDeg)
    {
        planetElements[idx] = { aAU * AU, e, i, Om, om, M0, nDegPerDay, settings::epochJD_J2000 };
        planetRotationSpeedDeg.push_back(rotSpeedDeg);
        planetObliquityDeg.push_back(obliqDeg);
    };
    if (planets.size() >= 9)
    {
        // Indexing: 0 Sun, 1 Earth, 2 Mercury, 3 Venus, 4 Mars, 5 Jupiter, 6 Saturn, 7 Uranus, 8 Neptune
        make(2, 0.387098, 0.205630, 7.00487, 48.33167, 29.12478, 174.79588, 360.0/87.969, settings::mercuryRotationSpeed, settings::obliqMercury);
        make(3, 0.723332, 0.006772, 3.39471, 76.68069, 54.85229, 50.41611, 360.0/224.701, settings::venusRotationSpeed,   settings::obliqVenus);
        make(1, 1.000000, 0.016710, 0.00005, -11.26064, 102.94719, 357.51716, 360.0/365.256, settings::earthRotationSpeed,   settings::obliqEarth);
        make(4, 1.523662, 0.093412, 1.85061, 49.57854, 336.04084, 19.41248, 360.0/686.980, settings::marsRotationSpeed,    settings::obliqMars);
        make(5, 5.203363, 0.048393, 1.30530, 100.55615, 14.75385, 20.02019, 360.0/4332.59, settings::jupiterRotationSpeed, settings::obliqJupiter);
        make(6, 9.537070, 0.054151, 2.48446, 113.71504, 92.43194, 317.02069, 360.0/10759.22, settings::saturnRotationSpeed,  settings::obliqSaturn);
        make(7, 19.191264, 0.047168, 0.76986, 74.22988, 170.96424, 142.23860, 360.0/30688.5, settings::uranusRotationSpeed,  settings::obliqUranus);
        make(8, 30.068964, 0.008586, 1.76917, 131.72169, 44.97135, 256.22800, 360.0/60182.0, settings::neptuneRotationSpeed, settings::obliqNeptune);

        // Ensure Mercury's perihelion clears the Sun radius with a small safety margin
        if (planetElements.size() > 2)
        {
            double peri = planetElements[2].a * (1.0 - planetElements[2].e);
            double minClear = (double)settings::sunScale + 6.0; // margin
            if (peri < minClear)
            {
                planetElements[2].a = minClear / (1.0 - planetElements[2].e);
            }
        }
    }
    //Setup the lighting in the shaders.
    window.UseShader(defaultShader);
    defaultShader.SendUniform<glm::vec3>("viewPosition", camera.GetPosition());
    defaultShader.SendUniform<float>("timeSeconds", lastTime);
    defaultShader.SendUniform<glm::vec3>("lightPosition", { 0.0f,0.0f,0.0f });
    defaultShader.SendUniform<glm::vec3>("ambientColor", settings::ambientColor);
    defaultShader.SendUniform<glm::vec3>("sunlightColor", settings::sunlightColor);
    window.UseShader(earthShader);
    earthShader.SendUniform<glm::vec3>("ambientColor", settings::earthAmbientColor);
    earthShader.SendUniform<glm::vec3>("sunlightColor", settings::sunlightColor);
    earthShader.SendUniform<glm::vec3>("lightPosition", { 0.0f,0.0f,0.0f });
    earthShader.SendUniform<float>("specularStrength", settings::earthSpecularStrength);
    earthShader.SendUniform<float>("specularShininess", settings::earthSpecularShininess);
    earthShader.SendUniform<int>("textureEarth", 0); //Assign locations to the texture samplers.
    earthShader.SendUniform<int>("textureEarthNight", 1);
    earthShader.SendUniform<int>("textureSpecular", 2);
    earthShader.SendUniform<int>("textureClouds", 3);
    // overlay shader lighting
    window.UseShader(overlayShader);
    overlayShader.SendUniform<glm::vec3>("ambientColor", settings::earthAmbientColor);
    overlayShader.SendUniform<glm::vec3>("sunlightColor", settings::sunlightColor);
    overlayShader.SendUniform<glm::vec3>("lightPosition", { 0.0f,0.0f,0.0f });

    // Create a simple ring mesh (unit ring in XZ plane) - positions and texcoords
    {
        std::vector<glm::vec3> pos;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> nrm;
        const int segments = 128;
        const float innerR = settings::saturnRingInner / settings::saturnRingOuter; // normalized inner radius based on settings
        const float outerR = 1.0f; // normalized outer radius
        for (int i = 0; i < segments; ++i)
        {
            float a0 = (float)i * 6.28318530718f / segments;
            float a1 = (float)(i+1) * 6.28318530718f / segments;
            float c0 = cos(a0), s0 = sin(a0);
            float c1 = cos(a1), s1 = sin(a1);
            // quad two triangles: outer0, inner0, inner1 and outer0, inner1, outer1
            glm::vec3 o0 = { outerR*c0, 0.0f, outerR*s0 };
            glm::vec3 i0 = { innerR*c0, 0.0f, innerR*s0 };
            glm::vec3 o1 = { outerR*c1, 0.0f, outerR*s1 };
            glm::vec3 i1 = { innerR*c1, 0.0f, innerR*s1 };
            // simple radial UV: r mapped to [0,1], angle to [0,1]
            // Radial mapping only: sample the ring texture horizontally from rightmost (inner) to leftmost (outer).
            // We'll map radius to U and ignore V (sample midline).
            // Avoid sampling extreme edges of the PNG where premultiplied alpha/bleed is visible
            float uInner = 0.98f; // rightmost of the PNG (clamped)
            float uOuter = 0.02f; // leftmost of the PNG (clamped)
            float uInnerMapped = glm::mix(uInner, uOuter, innerR);
            glm::vec2 uvo0 = { uOuter, 0.5f };
            glm::vec2 uvi0 = { uInnerMapped, 0.5f };
            glm::vec2 uvo1 = { uOuter, 0.5f };
            glm::vec2 uvi1 = { uInnerMapped, 0.5f };
            glm::vec3 up = {0,1,0};
            // tri1
            pos.push_back(o0); uv.push_back(uvo0); nrm.push_back(up);
            pos.push_back(i0); uv.push_back(uvi0); nrm.push_back(up);
            pos.push_back(i1); uv.push_back(uvi1); nrm.push_back(up);
            // tri2
            pos.push_back(o0); uv.push_back(uvo0); nrm.push_back(up);
            pos.push_back(i1); uv.push_back(uvi1); nrm.push_back(up);
            pos.push_back(o1); uv.push_back(uvo1); nrm.push_back(up);
        }
        ringMesh = Mesh(pos, uv, nrm);
    }

    // Generate asteroid belt (rescaled by Kepler AU so it sits between Mars and Jupiter)
    {
        asteroids.reserve(settings::asteroidCount);
        // Pre-generate a small pool of irregular icosphere meshes, then assign randomly per asteroid
        {
            asteroidMeshes.reserve(4);
            asteroidMeshes.push_back(Mesh::GenerateCraggyAsteroid(1, 1.0f, 0.15f, 3.0f, 11));
            asteroidMeshes.push_back(Mesh::GenerateCraggyAsteroid(2, 1.0f, 0.18f, 2.0f, 37));
            asteroidMeshes.push_back(Mesh::GenerateCraggyAsteroid(2, 1.2f, 0.2f, 2.5f, 73));
            asteroidMeshes.push_back(Mesh::GenerateCraggyAsteroid(3, 0.9f, 0.22f, 3.0f, 101));
        }
        std::mt19937 rng(12345);
        std::uniform_real_distribution<float> r01(0.0f, 1.0f);
        std::uniform_real_distribution<float> rSign(-1.0f, 1.0f);
        float asteroidBeltInnerWorld = 2.2f * settings::keplerAUScale;
        float asteroidBeltOuterWorld = 3.2f * settings::keplerAUScale;
        for (int i = 0; i < settings::asteroidCount; ++i)
        {
            float t = r01(rng);
            float radius = glm::mix(asteroidBeltInnerWorld, asteroidBeltOuterWorld, t);
            // Vary irregular scale along axes with tighter bounds for rocky appearance
            float baseScale = glm::mix(settings::asteroidMinScale, settings::asteroidMaxScale, r01(rng));
            // Random deviations from base to create rocky, lumpy shape (not bean-like)
            float sx = baseScale * glm::mix(0.7f, 1.3f, r01(rng));
            float sy = baseScale * glm::mix(0.7f, 1.3f, r01(rng));
            float sz = baseScale * glm::mix(0.7f, 1.3f, r01(rng));
            float angleDeg = r01(rng) * 360.0f;
            // Kepler-like speed: v ~ r^(-3/2) so angular speed ~ r^(-3/2)
            float angularSpeedDeg = settings::asteroidBaseAngularSpeed * powf(radius / asteroidBeltInnerWorld, -settings::asteroidSpeedExponent);
            // small vertical/lateral noise
            glm::vec3 offset = { 0.0f, (r01(rng) - 0.5f) * 2.0f * settings::asteroidBeltHalfThickness, 0.0f };
            // random spin
            glm::vec3 axis = glm::normalize(glm::vec3(rSign(rng), rSign(rng), rSign(rng)));
            float spinSpeed = glm::mix(10.0f, 60.0f, r01(rng));
            int meshIndex = int(r01(rng) * asteroidMeshes.size()) % asteroidMeshes.size();
            asteroids.push_back({ radius, angleDeg, angularSpeedDeg, {sx, sy, sz}, offset, r01(rng) * 360.0f, axis, spinSpeed, meshIndex });
        }
    }

    planetHudData.clear();
    planetHudData.resize(planets.size());
    if (planetHudData.size() > 1)
    {
        planetHudData[1] = { "EARTH", 149.6f, 29.78f, 23.93f, false };
    }
    if (planetHudData.size() > 2)
    {
        planetHudData[2] = { "MERCURY", 57.9f, 47.36f, 1407.5f, false };
    }
    if (planetHudData.size() > 3)
    {
        planetHudData[3] = { "VENUS", 108.2f, 35.02f, 5832.5f, true };
    }
    if (planetHudData.size() > 4)
    {
        planetHudData[4] = { "MARS", 227.9f, 24.07f, 24.62f, false };
    }
    if (planetHudData.size() > 5)
    {
        planetHudData[5] = { "JUPITER", 778.5f, 13.07f, 9.93f, false };
    }
    if (planetHudData.size() > 6)
    {
        planetHudData[6] = { "SATURN", 1434.0f, 9.68f, 10.7f, false };
    }
    if (planetHudData.size() > 7)
    {
        planetHudData[7] = { "URANUS", 2871.0f, 6.80f, 17.24f, true };
    }
    if (planetHudData.size() > 8)
    {
        planetHudData[8] = { "NEPTUNE", 4495.1f, 5.43f, 16.11f, false };
    }

    moonHudData = { "MOON", 0.384f, 1.022f, 655.7f, false };

    // Initialize N-body constants and Moon state
    {
        const double twoPi = 6.283185307179586;
        double rMoon = settings::moonOrbitRadius; // world units
        double periodMoonSec = (double)27.321661 * 86400.0; // sidereal
        muEarth = (float)((twoPi * twoPi) * (rMoon * rMoon * rMoon) / (periodMoonSec * periodMoonSec));

        double aEarth = settings::keplerAUScale * 1.0; // Earth a in world units
        double periodEarthSec = 365.256 * 86400.0;
        muSun = (float)((twoPi * twoPi) * (aEarth * aEarth * aEarth) / (periodEarthSec * periodEarthSec));

        double currentJD = settings::epochJD_J2000 + simulationTimeSec / 86400.0;
        glm::vec3 earthPos = orbitalPositionAtJD(planetElements[1], currentJD);
        // Approximate Earth inertial velocity via forward difference
        double epsSec = 10.0;
        glm::vec3 earthPos2 = orbitalPositionAtJD(planetElements[1], currentJD + epsSec / 86400.0);
        glm::vec3 earthVel = (earthPos2 - earthPos) / (float)epsSec;

        // Place Moon on +X from Earth; give it a circular tangential speed about Y axis
        moonRelPos = glm::vec3((float)rMoon, 0.0f, 0.0f);
        float vCirc = (float)(twoPi * rMoon / periodMoonSec);
        moonRelVel = glm::vec3(0.0f, 0.0f, vCirc);
        lastSimTimeSec = simulationTimeSec;
    }

    InitializeHudResources();
    InitializeOrbitPaths();
    InitializeFollowTargets();
    savedCameraPosition = camera.GetPosition();
    savedCameraYaw = camera.GetYaw();
    savedCameraPitch = camera.GetPitch();
}

void Game::InitializeFollowTargets()
{
    followTargets.clear();

    if (planets.size() > 1)
    {
        for (size_t i = 1; i < planets.size(); ++i)
        {
            followTargets.push_back({ FollowTargetEntry::Kind::Planet, i });
        }
    }

    // Always add the moon follow option last
    followTargets.push_back({ FollowTargetEntry::Kind::Moon, 0 });
}

void Game::InitializeHudResources()
{
    if (textVAO == 0)
    {
        glGenVertexArrays(1, &textVAO);
        glGenBuffers(1, &textVBO);
        glBindVertexArray(textVAO);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glBindVertexArray(0);
    }

    hudVertices.reserve(4096);
    hudProjection = glm::ortho(0.0f, static_cast<float>(settings::viewportWidth), static_cast<float>(settings::viewportHeight), 0.0f);
    stb_easy_font_spacing(1.0f);
}

void Game::InitializeOrbitPaths()
{
    const int segments = 512;
    const glm::vec3 palette[] = {
        {0.9f, 0.6f, 0.2f},
        {0.7f, 0.8f, 0.9f},
        {0.9f, 0.5f, 0.8f},
        {0.6f, 0.9f, 0.6f},
        {0.9f, 0.9f, 0.5f},
        {0.7f, 0.6f, 0.9f},
        {0.6f, 0.8f, 0.95f}
    };

    orbitPaths.clear();
    orbitPaths.resize(planets.size());

    for (size_t i = 1; i < planets.size() && i < planetElements.size(); ++i)
    {
        const OrbitalElements& el = planetElements[i];
        std::vector<float> vertices;
        vertices.reserve(static_cast<size_t>(segments) * 3);
        for (int s = 0; s < segments; ++s)
        {
            float t = (float)s / (float)segments;
            double nu = t * glm::two_pi<double>();
            double e = el.e;
            // radius from true anomaly
            double r = el.a * (1.0 - e * e) / (1.0 + e * cos(nu));
            double x_orb = r * cos(nu);
            double z_orb = r * sin(nu);
            glm::vec3 p = glm::vec3((float)x_orb, 0.0f, (float)z_orb);
            glm::mat4 R(1.0f);
            R = glm::rotate(R, (float)glm::radians((float)el.OmegaDeg), glm::vec3(0,1,0));
            R = glm::rotate(R, (float)glm::radians((float)el.iDeg),   glm::vec3(1,0,0));
            R = glm::rotate(R, (float)glm::radians((float)el.omegaDeg),glm::vec3(0,1,0));
            glm::vec3 w = glm::vec3(R * glm::vec4(p, 1.0f));
            vertices.push_back(w.x);
            vertices.push_back(w.y);
            vertices.push_back(w.z);
        }

        OrbitPath& path = orbitPaths[i];
        path.radius = (float)el.a;
        path.vertexCount = segments;
        path.color = palette[(i - 1) % (sizeof(palette) / sizeof(palette[0]))];

        glGenVertexArrays(1, &path.vao);
        glGenBuffers(1, &path.vbo);
        glBindVertexArray(path.vao);
        glBindBuffer(GL_ARRAY_BUFFER, path.vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindVertexArray(0);
    }

    // Moon orbit path (circle around Earth)
    {
        float radius = settings::moonOrbitRadius;
        std::vector<float> vertices;
        vertices.reserve(static_cast<size_t>(segments) * 3);
        for (int s = 0; s < segments; ++s)
        {
            float angle = (static_cast<float>(s) / segments) * glm::two_pi<float>();
            float x = radius * std::cos(angle);
            float z = radius * std::sin(angle);
            vertices.push_back(x);
            vertices.push_back(0.0f);
            vertices.push_back(z);
        }

        moonOrbitPath.radius = radius;
        moonOrbitPath.vertexCount = segments;
        moonOrbitPath.color = glm::vec3(0.85f, 0.85f, 0.95f);

        glGenVertexArrays(1, &moonOrbitPath.vao);
        glGenBuffers(1, &moonOrbitPath.vbo);
        glBindVertexArray(moonOrbitPath.vao);
        glBindBuffer(GL_ARRAY_BUFFER, moonOrbitPath.vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindVertexArray(0);
    }
}

void Game::CycleFollowTarget()
{
    if (followTargets.empty())
    {
        return;
    }

    if (followTargetIndex < 0)
    {
        savedCameraPosition = camera.GetPosition();
        savedCameraYaw = camera.GetYaw();
        savedCameraPitch = camera.GetPitch();
        hasSavedCameraPose = true;
        followTargetIndex = 0;
    }
    else
    {
        ++followTargetIndex;
        if (followTargetIndex >= static_cast<int>(followTargets.size()))
        {
            followTargetIndex = -1;
            if (hasSavedCameraPose)
            {
                camera.SetPose(savedCameraPosition, savedCameraYaw, savedCameraPitch);
            }
            followHudText.clear();
        }
    }
}

void Game::UpdateFollowCamera()
{
    if (followTargetIndex < 0 || followTargetIndex >= static_cast<int>(followTargets.size()))
    {
        return;
    }

    const FollowTargetEntry& entry = followTargets[followTargetIndex];
    glm::vec3 targetPos = GetFollowTargetPosition(entry);
    glm::vec3 cameraPos = ComputeFollowCameraPosition(entry, targetPos);
    camera.LookAt(cameraPos, targetPos);
    UpdateFollowHud(entry);
}

void Game::UpdateFollowHud(const FollowTargetEntry& entry)
{
    const PlanetHudInfo* info = nullptr;
    if (entry.kind == FollowTargetEntry::Kind::Planet)
    {
        if (entry.index < planetHudData.size())
        {
            info = &planetHudData[entry.index];
        }
    }
    else
    {
        info = &moonHudData;
    }

    if (!info)
    {
        followHudText.clear();
        return;
    }

    std::ostringstream oss;
    oss << info->name << '\n';

    oss << std::fixed << std::setprecision(1);
    oss << "ORBIT RADIUS: " << info->orbitRadiusMillionKm << " MILLION KM\n";
    oss << "ORBIT SPEED: " << info->orbitSpeedKmPerSec << " KM/S\n";

    double hours = static_cast<double>(info->rotationPeriodHours);
    bool retrograde = info->retrograde || hours < 0.0;
    double absHours = std::abs(hours);

    oss << "ROTATION PERIOD: ";
    if (absHours >= 48.0)
    {
        double days = absHours / 24.0;
        oss << std::setprecision(2) << days << " DAYS";
    }
    else
    {
        oss << std::setprecision(1) << absHours << " H";
    }

    if (retrograde)
    {
        oss << " (RETROGRADE)";
    }
    oss << '\n';

    followHudText = oss.str();
}

void Game::DrawFollowHud()
{
    if (followTargetIndex < 0 || followHudText.empty() || textVAO == 0)
    {
        return;
    }

    const size_t estimatedVertices = std::max<size_t>(followHudText.size() * 210, 210);
    if (hudVertices.size() < estimatedVertices * 2)
    {
        hudVertices.resize(estimatedVertices * 2);
    }

    const float textScale = 5.0f;
    const int rawHeightUnits = stb_easy_font_height(followHudText.c_str());
    const float blockHeight = rawHeightUnits * textScale;
    const float margin = 24.0f;
    glm::vec2 origin(margin, static_cast<float>(settings::viewportHeight) - margin - blockHeight);

    // Generate unscaled glyph geometry at origin
    int vertexCount = stb_easy_font_print(0.0f, 0.0f, followHudText.c_str(), nullptr,
        hudVertices.data(), static_cast<int>(hudVertices.size() * sizeof(float)));

    if (vertexCount <= 0)
    {
        return;
    }

    for (int i = 0; i < vertexCount; ++i)
    {
        hudVertices[i * 2 + 0] = origin.x + hudVertices[i * 2 + 0] * textScale;
        hudVertices[i * 2 + 1] = origin.y + hudVertices[i * 2 + 1] * textScale;
    }

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    window.UseShader(textShader);
    textShader.SendUniform<glm::mat4>("MVP", hudProjection);
    textShader.SendUniform<glm::vec4>("textColor", glm::vec4(1.0f));

    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 2 * sizeof(float), hudVertices.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void Game::DrawOrbitPaths(const glm::mat4& projection, const glm::mat4& viewMatrix)
{
    if (!showOrbitPaths)
    {
        return;
    }

    window.UseShader(orbitShader);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glLineWidth(0.75f);

    auto sendPath = [&](const OrbitPath& path, const glm::mat4& model, bool highlighted)
    {
        if (path.vertexCount == 0 || path.vao == 0)
        {
            return;
        }
        glm::vec3 color = highlighted ? glm::mix(path.color, glm::vec3(1.0f), 0.35f) : path.color;
        float opacity = highlighted ? 0.55f : 0.35f;
        orbitShader.SendUniform<glm::mat4>("MVP", projection * viewMatrix * model);
        orbitShader.SendUniform<glm::vec3>("color", color);
        orbitShader.SendUniform<float>("opacity", opacity);
        glBindVertexArray(path.vao);
        glDrawArrays(GL_LINE_LOOP, 0, path.vertexCount);
    };

    const bool followActive = followTargetIndex >= 0 && followTargetIndex < static_cast<int>(followTargets.size());
    const FollowTargetEntry* activeEntry = followActive ? &followTargets[followTargetIndex] : nullptr;

    for (size_t i = 1; i < orbitPaths.size(); ++i)
    {
        const OrbitPath& path = orbitPaths[i];
        if (path.vertexCount == 0)
        {
            continue;
        }
        glm::mat4 model(1.0f);
        bool highlighted = activeEntry && activeEntry->kind == FollowTargetEntry::Kind::Planet && activeEntry->index == i;
        sendPath(path, model, highlighted);
    }

    if (moonOrbitPath.vertexCount > 0)
    {
        glm::mat4 model = glm::mat4(1.0f);
        if (planets.size() > 1)
        {
            glm::vec3 earthPos = GetPlanetWorldPosition(1);
            model = glm::translate(model, earthPos);
        }
        bool highlighted = activeEntry && activeEntry->kind == FollowTargetEntry::Kind::Moon;
        sendPath(moonOrbitPath, model, highlighted);
    }

    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glLineWidth(1.0f);
}

glm::vec3 Game::GetPlanetWorldPosition(size_t index) const
{
    if (index >= planets.size())
    {
        return glm::vec3(0.0f);
    }
    const glm::mat4& model = planets[index].GetModelMatrix();
    glm::vec4 world = model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    return glm::vec3(world);
}

glm::vec3 Game::GetFollowTargetPosition(const FollowTargetEntry& entry) const
{
    if (entry.kind == FollowTargetEntry::Kind::Planet)
    {
        return GetPlanetWorldPosition(entry.index);
    }

    glm::vec4 world = moonModelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    return glm::vec3(world);
}

float Game::GetFollowTargetRadius(const FollowTargetEntry& entry) const
{
    if (entry.kind == FollowTargetEntry::Kind::Planet)
    {
        if (entry.index < planets.size())
        {
            return planets[entry.index].GetScale();
        }
        return settings::earthScale;
    }
    return settings::moonScale;
}

glm::vec3 Game::ComputeFollowCameraPosition(const FollowTargetEntry& entry, const glm::vec3& targetPos) const
{
    float radius = GetFollowTargetRadius(entry);
    float distance = std::max(radius * 3.0f, radius + 25.0f);

    glm::vec3 referencePos = glm::vec3(0.0f);
    if (entry.kind == FollowTargetEntry::Kind::Moon)
    {
        referencePos = GetPlanetWorldPosition(1); // Earth index.
    }

    glm::vec3 radialDir = targetPos - referencePos;
    float radialLength = glm::length(radialDir);
    if (radialLength < 1e-4f)
    {
        radialDir = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    else
    {
        radialDir /= radialLength;
    }

    glm::vec3 upDir = Camera::worldUp;
    glm::vec3 sideDir = glm::normalize(glm::cross(upDir, radialDir));
    if (glm::length(sideDir) < 1e-4f)
    {
        sideDir = glm::normalize(glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), radialDir));
        if (glm::length(sideDir) < 1e-4f)
        {
            sideDir = glm::vec3(1.0f, 0.0f, 0.0f);
        }
    }

    glm::vec3 elevatedDir = glm::normalize(glm::cross(radialDir, sideDir));

    glm::vec3 viewingDir = (entry.kind == FollowTargetEntry::Kind::Moon) ? radialDir : -radialDir;
    glm::vec3 offset = viewingDir * distance + elevatedDir * radius * 0.5f + sideDir * radius * 0.35f;
    return targetPos + offset;
}

glm::mat4 Game::BuildMoonModelMatrix() const
{
    if (planets.size() <= 1)
    {
        return glm::mat4(1.0f);
    }

    glm::vec3 earthPos = GetPlanetWorldPosition(1);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, earthPos);
    // Moon orbit around Earth in Earth's Y plane approximation
    model = glm::rotate(model, glm::radians(moonOrbitDeg), glm::vec3(0, 1, 0));
    model = glm::translate(model, glm::vec3(settings::moonOrbitRadius, 0.0f, 0.0f));
    // Tidal locking
    model = glm::rotate(model, glm::radians(-moonOrbitDeg), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(settings::moonScale));
    return model;
}

void Game::Tick()
{
    //Measure the time that has passed since the previous frame.
    float now = window.GetElapsedTime();
    float deltatime = now - lastTime;
    lastTime = now;

    window.ClearBuffers();  //Clears the color and depth buffers.
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
    //Logic update happens here.
    //Check if window should be closed.
    if(window.IsKeyPressed(settings::exitKey))
    {
        window.Close();
    }
    //Update camera rotation.
    glm::vec2 mousePosition = window.GetMousePosition();
    glm::vec2 cameraRotationOffset{ mousePosition.x - lastMousePosition.x, lastMousePosition.y - mousePosition.y };
    lastMousePosition = mousePosition;

    if (window.IsKeyPressedOnce(settings::cameraCycleKey))
    {
        CycleFollowTarget();
    }

    bool followActive = followTargetIndex >= 0;

    if (!followActive)
    {
        camera.Rotate(cameraRotationOffset);
        //Update camera position.
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
    }

    //Update wireframe mode.
    if (window.IsKeyPressedOnce(settings::wireframeModeKey))
    {
        window.ToggleWireframe();
    }
    if (window.IsKeyPressedOnce(settings::orbitToggleKey))
    {
        showOrbitPaths = !showOrbitPaths;
    }
    //Update camera speed.
    if (!followActive && window.IsKeyPressed(settings::cameraSpeedupKey))
    {
        camera.AddMovementSpeed(settings::cameraSpeedupRate);
    }
    if (!followActive && window.IsKeyPressed(settings::cameraSlowdownKey))
    {
        camera.AddMovementSpeed(-settings::cameraSpeedupRate);
    }
    if (window.IsKeyPressedOnce(settings::pauseKey))
    {
        isPaused = !isPaused;
    }

    if (!isPaused)
    {
        // Advance shader animation time only when running
        shaderTime += deltatime;
        // Advance simulation JD for Keplerian orbits
        simulationTimeSec += (double)deltatime * (double)timeSpeed * (double)settings::keplerDaysPerSecond * 86400.0;
        // Update Sun (index 0) legacy spin
        if (!planets.empty())
        {
            planets[0].Update(deltatime * timeSpeed);
        }
        // Accumulate self rotation for planets (1..8); use positive speed magnitude and encode
        // retrograde solely via obliquity (>90 deg means spin axis flipped)
        for (size_t i = 1; i < planets.size(); ++i)
        {
            if (i < planetRotationSpeedDeg.size())
            {
                // Simply accumulate rotation using configured speed
                // Negative speed in settings means retrograde rotation
                planetSelfRotationDeg[i] += planetRotationSpeedDeg[i] * deltatime * timeSpeed;
                // Normalize angle
                while (planetSelfRotationDeg[i] > 360.0f) planetSelfRotationDeg[i] -= 360.0f;
                while (planetSelfRotationDeg[i] < -360.0f) planetSelfRotationDeg[i] += 360.0f;
            }
        }
        // Update moon orbit angle only when not paused
        moonOrbitDeg += settings::moonOrbitSpeed * deltatime;
        if (moonOrbitDeg > 360.0f) moonOrbitDeg -= 360.0f;

        // Update asteroid angles
        for (auto& a : asteroids)
        {
            a.angleDeg += a.angularSpeedDeg * deltatime * timeSpeed;
            if (a.angleDeg > 360.0f) a.angleDeg -= 360.0f;
            a.spinDeg += a.spinSpeedDeg * deltatime * timeSpeed;
            if (a.spinDeg > 360.0f) a.spinDeg -= 360.0f;
        }

        //Update the time speed.
        if (window.IsKeyPressed(settings::timeSpeedupKey))
            timeSpeed += settings::timeAdjustSpeed;
        if (window.IsKeyPressed(settings::timeSlowdownKey))
            timeSpeed -= settings::timeAdjustSpeed;
    }

    // Build planet transforms from Kepler orbits (1..8)
    double currentJD = settings::epochJD_J2000 + simulationTimeSec / 86400.0;
    for (size_t i = 1; i < planets.size() && i < planetElements.size(); ++i)
    {
        const OrbitalElements& el = planetElements[i];
        glm::vec3 pos = orbitalPositionAtJD(el, currentJD);
        float scale = planets[i].GetScale();
        float obq = (i < planetObliquityDeg.size()) ? planetObliquityDeg[i] : 0.0f;
        float selfDeg = (i < planetSelfRotationDeg.size()) ? planetSelfRotationDeg[i] : 0.0f;

        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        glm::mat4 tiltMat = glm::rotate(glm::mat4(1.0f), glm::radians(obq), glm::vec3(1, 0, 0));
        glm::vec3 spinAxis = glm::vec3(tiltMat * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
        spinAxis = glm::normalize(spinAxis);

        model = glm::rotate(model, glm::radians(selfDeg), spinAxis);
        model = model * tiltMat;
        model = glm::scale(model, glm::vec3(scale));

        planets[i].SetModelMatrix(model);
        planets[i].SetSelfRotationDegrees(selfDeg);
    }

    // N-body integrate Moon around Earth (+Sun)
    {
        double dtSim = simulationTimeSec - lastSimTimeSec;
        lastSimTimeSec = simulationTimeSec;
        // Integrate using small fixed steps for stability
        const double maxStep = 600.0; // 10 min sim step for stability
        while (dtSim > 1e-6)
        {
            double h = std::min(dtSim, maxStep);
            // Acceleration in Earth's frame
            float dist = glm::length(moonRelPos) + 1e-6f;
            glm::vec3 acc = -(muEarth) * moonRelPos / (dist * dist * dist);
            // Leapfrog step in relative frame
            moonRelVel += (float)(0.5 * h) * acc;
            moonRelPos += (float)h * moonRelVel;
            dist = glm::length(moonRelPos) + 1e-6f;
            acc = -(muEarth) * moonRelPos / (dist * dist * dist);
            moonRelVel += (float)(0.5 * h) * acc;
            dtSim -= h;
        }
        // Safety: if the Moon drifted or became NaN, reinitialize to circular
        glm::vec3 rNow = moonRelPos;
        float rLen = glm::length(rNow);
        if (!(std::isfinite(rNow.x) && std::isfinite(rNow.y) && std::isfinite(rNow.z)) || rLen < settings::moonOrbitRadius * 0.5f || rLen > settings::moonOrbitRadius * 2.0f)
        {
            double periodMoonSec = (double)27.321661 * 86400.0;
            float vCirc = (float)(6.28318530718 * settings::moonOrbitRadius / periodMoonSec);
            moonRelPos = glm::vec3(settings::moonOrbitRadius, 0.0f, 0.0f);
            moonRelVel = glm::vec3(0.0f, 0.0f, vCirc);
        }
        glm::vec3 earthPosNow = GetPlanetWorldPosition(1);
        glm::vec3 moonPosWorld = earthPosNow + moonRelPos;
        glm::mat4 model(1.0f);
        model = glm::translate(model, moonPosWorld);
        model = glm::scale(model, glm::vec3(settings::moonScale));
        moonModelMatrix = model;
        moon.SetModelMatrix(moonModelMatrix);
    }
    UpdateFollowCamera();
}

void Game::Draw(float deltatime)
{
    //Drawing happens here.
    glm::mat4 projection = camera.GetPerspectiveMatrix();
    glm::mat4 viewMatrix = camera.GetViewMatrix();

    //Draw the planets (except for the sun and the earth) using the default shader (normal lighting).
    window.UseShader(defaultShader);
    defaultShader.SendUniform<glm::vec3>("viewPosition", camera.GetPosition());
    // Provide stable time to shader effects regardless of pause toggles
    defaultShader.SendUniform<float>("timeSeconds", shaderTime);
    for (size_t i = 2; i < planets.size(); ++i)
    {
        // Enable flow only for gaseous giants (Jupiter=5, Saturn=6, Uranus=7, Neptune=8)
        float flow = (i>=5 ? 1.0f : 0.0f);
        defaultShader.SendUniform<float>("flowAmount", flow);
        glm::mat4 model = planets[i].GetModelMatrix();
        defaultShader.SendUniform<glm::mat4>("MVP", projection * viewMatrix * model);
        defaultShader.SendUniform<glm::mat4>("modelMatrix", model);
        defaultShader.SendUniform<glm::mat3>("normalMatrix", planets[i].GetNormalMatrix());
        window.DrawActor(sphereMesh, planetTextures[i]);
    }
    //Draw the earth using its own shader.
    window.UseShader(earthShader);
    earthShader.SendUniform<glm::mat4>("MVP", projection * viewMatrix * planets[1].GetModelMatrix());
    earthShader.SendUniform<glm::mat4>("modelMatrix", planets[1].GetModelMatrix());
    earthShader.SendUniform<glm::mat3>("normalMatrix", planets[1].GetNormalMatrix());
    earthShader.SendUniform<glm::vec3>("viewPosition", camera.GetPosition());
    window.DrawActor(sphereMesh, planetTextures[1], earthNightTexture, earthSpecularTexture, earthCloudsTexture);

    // Venus atmosphere overlay using overlay shader
    // planet indices: 0 Sun, 1 Earth, 2 Mercury, 3 Venus, 4 Mars, 5 Jupiter, 6 Saturn, 7 Uranus, 8 Neptune
    const size_t venusIndex = 3;
    window.UseShader(overlayShader);
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // Build a model matrix matching Venus position/scale, with slight extra scale and a slow rotation
        glm::mat4 model = glm::mat4(1.0f);
        // Reconstruct from planet transform: orbit and base rotations reproduced via composition is complex.
        // Simpler: use planet's model then add slight scale and an extra yaw rotation.
        model = planets[venusIndex].GetModelMatrix();
        // Undo self-rotation effect on normals not necessary for overlay; we just scale slightly and rotate a bit more.
        model = glm::scale(model, glm::vec3(settings::venusAtmosphereScale));
        // Visible haze; pale warm tint. Alpha kept moderate to avoid washing the albedo
        overlayShader.SendUniform<float>("overlayAlpha", 0.55f);
        overlayShader.SendUniform<glm::vec3>("colorTint", glm::vec3(1.0f, 0.96f, 0.9f));
        overlayShader.SendUniform<glm::mat4>("MVP", projection * viewMatrix * model);
        overlayShader.SendUniform<glm::mat4>("modelMatrix", model);
        overlayShader.SendUniform<glm::mat3>("normalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
        window.DrawActor(sphereMesh, venusAtmosphereTexture);
        glDisable(GL_BLEND);
    }

    // Saturn rings overlay using ring mesh and overlay shader
    const size_t saturnIndex = 6;
    {
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 saturnPos = GetPlanetWorldPosition(saturnIndex);
        model = glm::translate(model, saturnPos);
        model = glm::scale(model, glm::vec3(settings::saturnScale));
        // Tilt ring plane around Z (equatorial tilt)
        model = glm::rotate(model, glm::radians(settings::saturnRingTiltDegrees), glm::vec3(0,0,1));
        // Apply ring outer radius and thin in Y
        model = glm::scale(model, glm::vec3(settings::saturnRingOuter, 0.001f, settings::saturnRingOuter));
        window.UseShader(overlayShader);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        overlayShader.SendUniform<float>("overlayAlpha", 0.7f);
        overlayShader.SendUniform<glm::vec3>("colorTint", glm::vec3(1.0f));
        // No UV wobble; rotation can be simulated by slightly rotating the model around Y when not paused
        if (!isPaused)
        {
            model = glm::rotate(model, glm::radians(settings::saturnRingAngularSpeed) * deltatime * timeSpeed, glm::vec3(0,1,0));
        }
        overlayShader.SendUniform<glm::mat4>("MVP", projection * viewMatrix * model);
        overlayShader.SendUniform<glm::mat4>("modelMatrix", model);
        overlayShader.SendUniform<glm::mat3>("normalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
        window.DrawActor(ringMesh, saturnRingTexture);
        glDisable(GL_BLEND);
    }

    // Uranus rings (thin, smaller, vertical alignment) using same ring mesh
    const size_t uranusIndex = 7;
    {
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 uranusPos = GetPlanetWorldPosition(uranusIndex);
        model = glm::translate(model, uranusPos);
        model = glm::scale(model, glm::vec3(settings::uranusScale));
        // Vertical ring: 90 degrees about X
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1,0,0));
        // Smaller and thinner than Saturn's
        model = glm::scale(model, glm::vec3(settings::saturnRingOuter * 0.5f, 0.001f, settings::saturnRingOuter * 0.5f));
        window.UseShader(overlayShader);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        overlayShader.SendUniform<float>("overlayAlpha", 0.9f);
        // Darker tint for Uranus rings
        overlayShader.SendUniform<glm::vec3>("colorTint", glm::vec3(0.5f, 0.55f, 0.6f));
        overlayShader.SendUniform<glm::mat4>("MVP", projection * viewMatrix * model);
        overlayShader.SendUniform<glm::mat4>("modelMatrix", model);
        overlayShader.SendUniform<glm::mat3>("normalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
        window.DrawActor(ringMesh, uranusRingTexture);
        glDisable(GL_BLEND);
    }

    // Asteroid belt draw (after planets, before sun/skybox)
    window.UseShader(defaultShader);
    defaultShader.SendUniform<glm::vec3>("viewPosition", camera.GetPosition());
    for (const auto& a : asteroids)
    {
        glm::mat4 model = glm::mat4(1.0f);
        // Orbital rotation around Y, then translate to belt radius
        model = glm::rotate(model, glm::radians(a.angleDeg), glm::vec3(0,1,0));
        model = glm::translate(model, glm::vec3(a.radius, 0.0f, 0.0f));
        // Apply belt tilt and vertical offset
        model = glm::rotate(model, glm::radians(settings::asteroidBeltTiltDegrees), glm::vec3(1,0,0));
        model = glm::translate(model, a.offset);
        // Random local spin for sparkle
        model = glm::rotate(model, glm::radians(a.spinDeg), a.spinAxis);
        model = glm::scale(model, a.scale);
        defaultShader.SendUniform<glm::mat4>("MVP", projection * viewMatrix * model);
        defaultShader.SendUniform<glm::mat4>("modelMatrix", model);
        defaultShader.SendUniform<glm::mat3>("normalMatrix", glm::mat3(glm::transpose(glm::inverse(model))));
        // Use one of the irregular meshes, then scale to asteroid dimensions
        window.DrawActor(asteroidMeshes[a.meshIndex], asteroidTexture);
    }

    // Moon: orbit Earth and stay tidally locked (same face toward Earth)
    {
        glm::mat4 model = moon.GetModelMatrix();
        window.UseShader(defaultShader);
        defaultShader.SendUniform<glm::vec3>("viewPosition", camera.GetPosition());
        defaultShader.SendUniform<glm::mat4>("MVP", projection * viewMatrix * model);
        defaultShader.SendUniform<glm::mat4>("modelMatrix", model);
        defaultShader.SendUniform<glm::mat3>("normalMatrix", moon.GetNormalMatrix());
        window.DrawActor(sphereMesh, moonTexture);
    }
    // (cleanup) removed stray placeholder Venus code
    //Draw the sun and the skybox without lighting.
    window.UseShader(noLightShader);
    //Draw sun with no-light shader but animated UVs and brightness boost
    window.UseShader(noLightShader);
    noLightShader.SendUniform<glm::mat4>("MVP", projection * viewMatrix * planets[0].GetModelMatrix());
    // Use accumulated time so pausing doesn't snap animation phase
    noLightShader.SendUniform<float>("timeSeconds", shaderTime * 0.25f);
    window.DrawActor(sphereMesh, planetTextures[0]);
    //Draw skybox.
    viewMatrix = glm::mat4(glm::mat3(viewMatrix));//Remove the translation from the view matrix, we do not want our skybox to move around.
    // Keep same time uniform for skybox so it freezes consistently when paused
    noLightShader.SendUniform<float>("timeSeconds", shaderTime * 0.25f);
    noLightShader.SendUniform<glm::mat4>("MVP", projection * viewMatrix * skyBox.GetModelMatrix());
    window.DrawActor(sphereMesh, skyboxTexture);

    // Draw orbit guides last using the REAL camera view (not skybox view without translation)
    glm::mat4 viewMatrixForOrbits = camera.GetViewMatrix();
    DrawOrbitPaths(projection, viewMatrixForOrbits);

    DrawFollowHud();
}
