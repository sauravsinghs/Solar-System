// Mesh construction helpers and procedural generators.
// - GenerateIcoSphere: subdivided icosahedron projected to a sphere, with spherical UVs
// - GenerateCraggyAsteroid: applies multi-octave value noise displacement for rocky shapes
#include "Mesh.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include "objload.h"
namespace {
    struct Tri { int a,b,c; };
}

Mesh Mesh::GenerateIcoSphere(int subdivisions, float radius)
{
    // Icosahedron vertices
    const float t = (1.0f + sqrtf(5.0f)) * 0.5f;
    std::vector<glm::vec3> v = {
        {-1,  t,  0}, { 1,  t,  0}, {-1, -t,  0}, { 1, -t,  0},
        { 0, -1,  t}, { 0,  1,  t}, { 0, -1, -t}, { 0,  1, -t},
        { t,  0, -1}, { t,  0,  1}, {-t,  0, -1}, {-t,  0,  1}
    };
    for (auto &p : v) p = glm::normalize(p);
    std::vector<Tri> f = {
        {0,11,5},{0,5,1},{0,1,7},{0,7,10},{0,10,11},
        {1,5,9},{5,11,4},{11,10,2},{10,7,6},{7,1,8},
        {3,9,4},{3,4,2},{3,2,6},{3,6,8},{3,8,9},
        {4,9,5},{2,4,11},{6,2,10},{8,6,7},{9,8,1}
    };
    auto midpoint = [&](int a, int b, std::map<long long,int>& cache)->int{
        long long key = ((long long)std::min(a,b)<<32) | (long long)std::max(a,b);
        auto it = cache.find(key);
        if (it!=cache.end()) return it->second;
        glm::vec3 m = glm::normalize((v[a]+v[b]) * 0.5f);
        v.push_back(m);
        int idx = (int)v.size()-1;
        cache[key]=idx;
        return idx;
    };
    for (int s=0; s<subdivisions; ++s) {
        std::map<long long,int> cache;
        std::vector<Tri> nf;
        nf.reserve(f.size()*4);
        for (auto &tr: f) {
            int ab = midpoint(tr.a,tr.b,cache);
            int bc = midpoint(tr.b,tr.c,cache);
            int ca = midpoint(tr.c,tr.a,cache);
            nf.push_back({tr.a,ab,ca});
            nf.push_back({tr.b,bc,ab});
            nf.push_back({tr.c,ca,bc});
            nf.push_back({ab,bc,ca});
        }
        f.swap(nf);
    }
    // Build buffers (positions, trivial spherical UVs, vertex normals)
    std::vector<glm::vec3> pos; pos.reserve(f.size()*3);
    std::vector<glm::vec2> uv;  uv.reserve(f.size()*3);
    std::vector<glm::vec3> nrm; nrm.reserve(f.size()*3);
    for (auto &tr: f) {
        int ids[3] = {tr.a,tr.b,tr.c};
        for (int k=0;k<3;++k){
            glm::vec3 p = glm::normalize(v[ids[k]]);
            glm::vec3 n = p;
            float u = 0.5f + atan2f(p.z, p.x) / (2.0f*3.14159265f);
            float vtex = 0.5f - asinf(p.y)/3.14159265f;
            pos.push_back(p * radius);
            uv.push_back({u,vtex});
            nrm.push_back(n);
        }
    }
    return Mesh(pos, uv, nrm);
}

static float hash31(const glm::vec3& p)
{
    glm::vec3 q = glm::fract(p * 0.3183099f + glm::vec3(0.71f, 0.113f, 0.419f));
    glm::vec3 yzx = glm::vec3(q.y, q.z, q.x);
    q += glm::dot(q, yzx + glm::vec3(19.19f));
    return glm::fract((q.x + q.y) * q.z);
}

static float noise3D(const glm::vec3& p)
{
    // Simple value noise via hashing corners and trilinear mix
    glm::vec3 i = glm::floor(p);
    glm::vec3 f = p - i;
    glm::vec3 u = f * f * (3.0f - 2.0f * f);
    float n000 = hash31(i + glm::vec3(0,0,0));
    float n001 = hash31(i + glm::vec3(0,0,1));
    float n010 = hash31(i + glm::vec3(0,1,0));
    float n011 = hash31(i + glm::vec3(0,1,1));
    float n100 = hash31(i + glm::vec3(1,0,0));
    float n101 = hash31(i + glm::vec3(1,0,1));
    float n110 = hash31(i + glm::vec3(1,1,0));
    float n111 = hash31(i + glm::vec3(1,1,1));
    float nx00 = glm::mix(n000, n100, u.x);
    float nx01 = glm::mix(n001, n101, u.x);
    float nx10 = glm::mix(n010, n110, u.x);
    float nx11 = glm::mix(n011, n111, u.x);
    float nxy0 = glm::mix(nx00, nx10, u.y);
    float nxy1 = glm::mix(nx01, nx11, u.y);
    return glm::mix(nxy0, nxy1, u.z);
}

Mesh Mesh::GenerateCraggyAsteroid(int subdivisions, float radius, float amplitude, float frequency, unsigned int seed)
{
    Mesh base = GenerateIcoSphere(subdivisions, 1.0f);
    // Read back from base mesh buffers via temporary vectors used to construct Mesh
    // We don't have direct access; regenerate positions/normals similarly to GenerateIcoSphere
    const float t = (1.0f + sqrtf(5.0f)) * 0.5f;
    std::vector<glm::vec3> v = {
        {-1,  t,  0}, { 1,  t,  0}, {-1, -t,  0}, { 1, -t,  0},
        { 0, -1,  t}, { 0,  1,  t}, { 0, -1, -t}, { 0,  1, -t},
        { t,  0, -1}, { t,  0,  1}, {-t,  0, -1}, {-t,  0,  1}
    };
    for (auto &p : v) p = glm::normalize(p);
    std::vector<Tri> f = {
        {0,11,5},{0,5,1},{0,1,7},{0,7,10},{0,10,11},
        {1,5,9},{5,11,4},{11,10,2},{10,7,6},{7,1,8},
        {3,9,4},{3,4,2},{3,2,6},{3,6,8},{3,8,9},
        {4,9,5},{2,4,11},{6,2,10},{8,6,7},{9,8,1}
    };
    auto midpoint = [&](int a, int b, std::map<long long,int>& cache)->int{
        long long key = ((long long)std::min(a,b)<<32) | (long long)std::max(a,b);
        auto it = cache.find(key);
        if (it!=cache.end()) return it->second;
        glm::vec3 m = glm::normalize((v[a]+v[b]) * 0.5f);
        v.push_back(m);
        int idx = (int)v.size()-1;
        cache[key]=idx;
        return idx;
    };
    for (int s=0; s<subdivisions; ++s) {
        std::map<long long,int> cache;
        std::vector<Tri> nf;
        nf.reserve(f.size()*4);
        for (auto &tr: f) {
            int ab = midpoint(tr.a,tr.b,cache);
            int bc = midpoint(tr.b,tr.c,cache);
            int ca = midpoint(tr.c,tr.a,cache);
            nf.push_back({tr.a,ab,ca});
            nf.push_back({tr.b,bc,ab});
            nf.push_back({tr.c,ca,bc});
            nf.push_back({ab,bc,ca});
        }
        f.swap(nf);
    }
    std::vector<glm::vec3> pos; pos.reserve(f.size()*3);
    std::vector<glm::vec2> uv;  uv.reserve(f.size()*3);
    std::vector<glm::vec3> nrm; nrm.reserve(f.size()*3);
    // Seed variation inputs
    float freq = frequency;
    float amp = amplitude;
    for (auto &tr: f) {
        int ids[3] = {tr.a,tr.b,tr.c};
        for (int k=0;k<3;++k){
            glm::vec3 p = glm::normalize(v[ids[k]]);
            // Multi-octave value noise along normal direction
            float n = 0.0f, scale = 1.0f, fall = 0.5f;
            glm::vec3 np = p * freq + float(seed) * 0.01f;
            for (int o=0;o<4;++o){ n += noise3D(np)*scale; np *= 2.0f; scale *= fall; }
            float disp = 1.0f + amp * (n - 0.5f); // displace around unit radius
            glm::vec3 pp = glm::normalize(p) * (radius * disp);
            glm::vec3 nn = glm::normalize(pp);
            float u = 0.5f + atan2f(nn.z, nn.x) / (2.0f*3.14159265f);
            float vtex = 0.5f - asinf(nn.y)/3.14159265f;
            pos.push_back(pp);
            uv.push_back({u,vtex});
            nrm.push_back(nn);
        }
    }
    return Mesh(pos, uv, nrm);
}

Mesh::Mesh() noexcept
    : VAO(0), VBOVertex(0), VBOTexture(0), VBONormals(0), vertexCount(0)
{
}

Mesh::Mesh(std::string meshPath)
{
	//Load the object's vertex data from disk.
	std::vector<glm::vec3> vertexPositions;
	std::vector<glm::vec2> textureCoordinates;
	std::vector<glm::vec3> normals;
	loadOBJ(meshPath.c_str(), vertexPositions, textureCoordinates, normals);
	//Save the number of vertices.
	vertexCount = (int)vertexPositions.size();
	assert(vertexCount >= 3); //If assertion fails : Object could not be loaded or it does not contain enough vertices.
	//Setup the buffers for this object.
	//VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//VBOVertex
	glGenBuffers(1, &VBOVertex);
	glBindBuffer(GL_ARRAY_BUFFER, VBOVertex);
	glBufferData(GL_ARRAY_BUFFER, vertexPositions.size() * sizeof(glm::vec3), vertexPositions.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	//VBOTexture
	glGenBuffers(1, &VBOTexture);
	glBindBuffer(GL_ARRAY_BUFFER, VBOTexture);
	glBufferData(GL_ARRAY_BUFFER, textureCoordinates.size() * sizeof(glm::vec2), textureCoordinates.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	//VBONormals
	glGenBuffers(1, &VBONormals);
	glBindBuffer(GL_ARRAY_BUFFER, VBONormals);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(2);
}

Mesh::Mesh(const std::vector<glm::vec3>& vertexPositions,
           const std::vector<glm::vec2>& textureCoordinates,
           const std::vector<glm::vec3>& normals)
{
    //Save the number of vertices.
    vertexCount = (int)vertexPositions.size();
    assert(vertexCount >= 3);
    //Setup the buffers for this object.
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    // Positions
    glGenBuffers(1, &VBOVertex);
    glBindBuffer(GL_ARRAY_BUFFER, VBOVertex);
    glBufferData(GL_ARRAY_BUFFER, vertexPositions.size() * sizeof(glm::vec3), vertexPositions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    // Texcoords
    glGenBuffers(1, &VBOTexture);
    glBindBuffer(GL_ARRAY_BUFFER, VBOTexture);
    glBufferData(GL_ARRAY_BUFFER, textureCoordinates.size() * sizeof(glm::vec2), textureCoordinates.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
    // Normals
    glGenBuffers(1, &VBONormals);
    glBindBuffer(GL_ARRAY_BUFFER, VBONormals);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(2);
}

Mesh::Mesh(Mesh&& other) noexcept
	:
	VAO(other.VAO),
	VBOVertex(other.VBOVertex),
	VBOTexture(other.VBOTexture),
	VBONormals(other.VBONormals),
	vertexCount(other.vertexCount)
{
	other.VAO = 0;
	other.VBOVertex = 0;
	other.VBOTexture = 0;
	other.VBONormals = 0;
	other.vertexCount = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
	if (this != &other)
	{
		//Free the data held by this object.
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBOVertex);
		glDeleteBuffers(1, &VBOTexture);
		glDeleteBuffers(1, &VBONormals);
		//Pilfer the data from the other object.
		VAO = other.VAO;
		VBOVertex = other.VBOVertex;
		VBOTexture = other.VBOTexture;
		VBONormals = other.VBONormals;
		vertexCount = other.vertexCount;
		other.VAO = 0;
		other.VBOVertex = 0;
		other.VBOTexture = 0;
		other.VBONormals = 0;
		other.vertexCount = 0;
	}
	return *this;
}

Mesh::~Mesh() noexcept
{
	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBOVertex);
		glDeleteBuffers(1, &VBOTexture);
		glDeleteBuffers(1, &VBONormals);
	}
}

unsigned int Mesh::GetVAO() const
{
	return VAO;
}

int Mesh::GetVertexCount() const
{
	return vertexCount;
}
