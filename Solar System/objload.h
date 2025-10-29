#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#ifndef OBJLOAD_H
#define OBJLOAD_H

bool loadOBJ(
	const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
);

#endif



// 📘 Explanation:

// Ye header file hai jo loadOBJ function ka declaration deti hai.

// Function ka naam: loadOBJ

// Parameters:

// path: .obj file ka path (file name).

// out_vertices: output vector jisme sab vertices (3D points) store honge.

// out_uvs: output vector jisme texture mapping ke coordinates honge.

// out_normals: output vector jisme surface normals (light calculation ke liye) honge.

// bool return karta hai → agar file sahi load hui toh true, warna false.
