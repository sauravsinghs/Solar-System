#include "Orbital.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <algorithm>

static inline double deg2rad(double d) { return d * 0.017453292519943295; }
static inline double wrapRad(double r)
{
    const double twoPi = 6.283185307179586;
    r = fmod(r, twoPi);
    if (r < 0.0) r += twoPi;
    return r;
}

glm::vec3 orbitalPositionAtJD(const OrbitalElements& el, double jd)
{
    // Mean anomaly at time
    double dDays = jd - el.epochJD;
    double M = deg2rad(el.M0Deg + el.nDegPerDay * dDays);
    M = wrapRad(M);

    // Solve Kepler: M = E - e*sin(E)
    double E = M;
    for (int k = 0; k < 8; ++k)
    {
        double f = E - el.e * sin(E) - M;
        double fp = 1.0 - el.e * cos(E);
        E = E - f / fp;
    }

    double cosE = cos(E), sinE = sin(E);
    double sqrtOneMinusE2 = sqrt(std::max(0.0, 1.0 - el.e * el.e));
    double cosNu = (cosE - el.e) / (1.0 - el.e * cosE);
    double sinNu = (sqrtOneMinusE2 * sinE) / (1.0 - el.e * cosE);
    double nu = atan2(sinNu, cosNu);

    // Radius
    double r = el.a * (1.0 - el.e * cosE);

    // Position in orbital plane (choose XZ plane convention)
    double x_orb = r * cos(nu);
    double z_orb = r * sin(nu);

    glm::vec3 p = glm::vec3((float)x_orb, 0.0f, (float)z_orb);

    // Rotate by Ω (Y), i (X), ω (Y) to world (Y-up)
    glm::mat4 R(1.0f);
    R = glm::rotate(R, (float)deg2rad(el.OmegaDeg), glm::vec3(0, 1, 0));
    R = glm::rotate(R, (float)deg2rad(el.iDeg), glm::vec3(1, 0, 0));
    R = glm::rotate(R, (float)deg2rad(el.omegaDeg), glm::vec3(0, 1, 0));
    glm::vec4 pw = R * glm::vec4(p, 1.0f);
    return glm::vec3(pw);
}


