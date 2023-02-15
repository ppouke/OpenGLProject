#pragma once

#include <glad/glad.h>
#include <glm/matrix.hpp>
#include <glm/glm.hpp>

#include <string>
#include <vector>

using namespace std;

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 TexCoords;
};

struct Texture {
	unsigned int id;
	string type;
};

class Mesh {
public:
	vector <Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures) {
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		SetupMesh();
	}
	void Draw(Shader, &shader);

private:
	unsigned int VBO, VAO, EBO;
	void SetupMesh() {

	}
};


