#pragma once
#include <glm/glm.hpp>

// Analytic Keplerian orbit utilities (J2000-style elements, simple mean motion)
struct OrbitalElements
{
    double a;           // semi-major axis (scene units)
    double e;           // eccentricity
    double iDeg;        // inclination (degrees)
    double OmegaDeg;    // longitude of ascending node (degrees)
    double omegaDeg;    // argument of periapsis (degrees)
    double M0Deg;       // mean anomaly at epoch (degrees)
    double nDegPerDay;  // mean motion (deg/day)
    double epochJD;     // epoch in Julian Days
};

// Compute heliocentric position for given elements at Julian Date.
glm::vec3 orbitalPositionAtJD(const OrbitalElements& el, double jd);


