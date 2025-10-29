#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
//Represents a mesh, handles the loading from disk.
class Mesh
{
public:
	Mesh() noexcept; // default construct an empty mesh
	Mesh(std::string meshPath);					//Loads the vertex, texture and normal data from disk.
	Mesh(const std::vector<glm::vec3>& positions, const std::vector<glm::vec2>& texCoords, const std::vector<glm::vec3>& normals);
	static Mesh GenerateIcoSphere(int subdivisions, float radius);
	static Mesh GenerateCraggyAsteroid(int subdivisions, float radius, float amplitude, float frequency, unsigned int seed);
	Mesh(const Mesh& other) = delete;			//No copy construction allowed.
	Mesh& operator=(const Mesh& other) = delete;//No copy assignment allowed.
	Mesh(Mesh&& other) noexcept;				//Move constructor.
	Mesh& operator=(Mesh&& other) noexcept;		//Move assignment.
	~Mesh() noexcept;							//Frees the vertex, texture and normal VBOs.
	unsigned int GetVAO() const;
	int GetVertexCount() const;
private:
	int vertexCount;
	//The buffers for the mesh.
	unsigned int VAO;
	unsigned int VBOVertex;
	unsigned int VBOTexture;
	unsigned int VBONormals;
};
// mesh bas keval model ko dekha raha like planet ke models ko represent karta hai