#pragma once



#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <shader_s.h>
#include <string>
#include <vector>

#include <Includes/stb_image.h>


using namespace std;



enum PrimitiveType {
	Plane, Box, Sphere
};

class primitive {

public:
	const char* diffuseTex;
	const char* specularTex;
	

	primitive(PrimitiveType primitiveType, const char* diffuseTex = "Resources/Images/DefaultTextures/defaultDiffuse.jpg",
		const char* specularTex = "Resources/Images/DefaultTexture/defaultSpecular.jpg")
	{
		this->diffuseTex = diffuseTex;
		this->specularTex = specularTex;
		
		SetupMesh(primitiveType);
	}


	void Draw(Shader& shader) {


		//diffuse
		glActiveTexture(GL_TEXTURE0);
		shader.setInt("material.texture_diffuse1", 0);
		glBindTexture(GL_TEXTURE_2D, TextureFromFile(diffuseTex));

		//specular
		glActiveTexture(GL_TEXTURE1);
		shader.setInt("material.texture_specular1", 1);
		glBindTexture(GL_TEXTURE_2D, TextureFromFile(specularTex));


	}


private:

	unsigned int TextureFromFile(const char* path) {
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrChannels;
		unsigned char* data = stbi_load(path, &width, &height, &nrChannels);
		if (data) {
			GLenum format;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
				format = GL_RGBA;


			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else {
			std::cout << "Failed to load texture at " << path << std::endl;
			stbi_image_free(data);
		}	

		return textureID;
	}

private:
	unsigned int VBO, VAO, EBO;
	void SetupMesh(unsigned int primitiveType) {



		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		//vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		//normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		//texCoord
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);

	}


};