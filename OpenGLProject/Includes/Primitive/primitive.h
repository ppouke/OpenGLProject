#pragma once



#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <shader_s.h>
#include <string>
#include <vector>

#include <Includes/stb_image.h>

#include <Primitive/primitiveTypes.h>

#include <Model/mesh.h>
using namespace std;



class Primitive {

public:
	const char* diffuseTex;
	const char* specularTex;
	
	vector<float> vertices;
	vector<unsigned int> indices;
	vector<unsigned int> textures;

	bool usingIndices;

	Primitive(PrimitiveType primitiveType, const char* diffuseTex = "Resources/Images/DefaultTextures/defaultDiffuse.jpg",
		const char* specularTex = "Resources/Images/DefaultTextures/defaultSpecular.jpg")
	{
		this->diffuseTex = diffuseTex;
		this->specularTex = specularTex;
		this->usingIndices = true;
		
		SetupMesh(primitiveType);
	}


	void Draw(Shader& shader) {



		for (int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			if(i == 0){
				//diffuse
				glActiveTexture(GL_TEXTURE0);
				shader.setInt("material.texture_diffuse1", i);
				glBindTexture(GL_TEXTURE_2D, textures[i]);
			}
			else {
				//specular
				glActiveTexture(GL_TEXTURE1);
				shader.setInt("material.texture_specular1", i);
				glBindTexture(GL_TEXTURE_2D, textures[i]);
			}
		}


		glBindVertexArray(VAO);

		if (usingIndices)
			glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, vertices.size()/(sizeof(Vertex) / sizeof(GL_BYTE)));
		
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);



	}


private:


	//get as vector for dynamic size creation
	
	unsigned int TextureFromFile(const char* path) {
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrChannels;
		unsigned char* data = stbi_load(path, &width, &height, &nrChannels,0);
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
	void SetupMesh(PrimitiveType primType) {



		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);



		GetPrimitive(primType, vertices, indices);


		//check if using indices 
		if (!indices.empty()) usingIndices = true;
		else usingIndices = false;
		

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);


		if(usingIndices){
			glGenBuffers(1, &EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		}

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
		

		//textures
		
		textures.push_back(TextureFromFile(diffuseTex));
		textures.push_back(TextureFromFile(specularTex));

	}


};