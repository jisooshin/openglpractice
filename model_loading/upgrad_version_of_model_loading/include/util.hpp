#pragma once
#include <glad/glad.h>
#include <string>
#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <stdio.h>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.05f;
const float ZOOM = 45.0f;

enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

using namespace std;

// declaration
GLuint TextureFromFile(const char *path, const string &directory);
string format_stringi(string &&fmt, int idx);

class Shader
{
public:
	GLuint ID;
	Shader(const char *vertexPath, const char *fragPath);
	void use();
	void setBool(const string &name, bool value) const;
	void setInt(const string &name, int value) const;
	void setFloat(const string &name, float value) const;
	void setMat4(const string &name, glm::mat4 value) const;
	void setVec3(const string &name, glm::vec3 v) const;
};

class Camera
// 애초에 카메라 view 매트릭스를 만드는 것이 목적인 class
// rendering loop 안에서
{
public:
	glm::vec3 Position, Front, Up, Right, WorldUp;
	float Yaw, Pitch, MovementSpeed, MouseSensitivity, Zoom;
	Camera(
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = YAW,
		float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
							   MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	Camera(
		float posX, float posY, float posZ,
		float upX, float upY, float upZ,
		float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
								  MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
	}
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;
		Yaw += xoffset;
		Pitch += yoffset;

		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}
		updateCameraVectors();
	}
	void ProcessMouseScroll(float yoffset)
	{
		Zoom -= (float)yoffset;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 45.0f)
			Zoom = 45.0f;
	}

private:
	void updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

		Front = glm::normalize(front);
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};

struct Vertex
{
	// 일단 사용하기 쉽도록 Vertex struct을 하나 만들어 놓는다.
	glm::vec3 Position; // vertex 좌표
	glm::vec3 Normal;	// vertex 법선벡터
	glm::vec2 TexCoord; // (만약 있다면) 해당 vertex가 Texture 이미지의 어느 좌표에 속하는지에 대한 좌표
};

struct ColorProperty
{
	glm::vec3 ambient  = glm::vec3(1.0f);
	glm::vec3 diffuse  = glm::vec3(1.0f);
	glm::vec3 specular = glm::vec3(1.0f);
};

struct Texture
{
	GLuint id;	 // Gpu 에 올려놓는 Texture의 id
	string type; // specular 인지 diffuse 인지 등을 저장하는 type
	string path; // filePath (Texture를 나타내는 이미지 파일이 따로 있는 경우를 나타내기 위함인듯 )
	size_t materialIndex = 0;
	ColorProperty colorP;
	float shiness;
};

class Mesh
// vertex들이 모여 Mesh 된다. (일단 primitive는 triangle로)
// 여기서의 Mesh 는 작은 삼각형 하나가 될수도 있고 작은 3d 객체가 될수도 있다.
{
public:
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;

	Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
		: vertices(vertices), indices(indices), textures(textures) { setupMesh(); };
	void Draw(Shader &shader)
	{
		// Draw 함수에서는 이미 만들어진 Texture들을 그리는 것만 관장한다.
		int diffuseN = 1;
		int specularN = 1;
		for (size_t i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			string number;
			string name = textures[i].type;
			string uniformName;
			string uniformNameParam;
			if (name == "texture_diffuse")
			{
				uniformName = format_stringi("material[%i].diffuse", textures[i].materialIndex);
			}
			else if (name == "texture_specular")
			{
				uniformName = format_stringi("material[%i].specular", textures[i].materialIndex);
			}
			else if (name == "texture_bump")
			{
				uniformName = format_stringi("material[%i].bump", textures[i].materialIndex);
			}

			shader.setInt(uniformName.c_str(), (int)i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		// glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

private:
	GLuint VAO, VBO, EBO;
	void setupMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoord));
		glBindVertexArray(0);
	}
};

struct sumCoord
{
	int num_vertices = 0;
	double min_x = DBL_MAX;
	double max_x = DBL_MIN;
	double min_y = DBL_MAX;
	double max_y = DBL_MIN;
	double min_z = DBL_MAX;
	double max_z = DBL_MIN;
};

class Model
// 여러 mesh들이 모여 model이 된다
{
public:
	Model(string path) // 이 path 는 model을 담아두는 파일이 될 것
	{
		loadModel(path);
	}
	void Draw(Shader &shader)
	{
		for (auto mesh : meshes)
		{
			mesh.Draw(shader);
		}
	}
private:
	sumCoord sum_of_vertices;
	vector<Mesh> meshes;
	vector<Texture> textures_loaded;
	string directory;
	size_t materialIndex = 0;

	void loadModel(string path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(
			path,
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
		}
		directory = path.substr(0, path.find_last_of('/')); // 나중에 텍스쳐 불러올때 사용
		gettingNecessaryData(scene->mRootNode, scene);
		processNode(scene->mRootNode, scene);
	}
	
	void gettingNecessaryData_from_mesh(aiMesh *mesh, const aiScene *scene)
	{
		for (size_t i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 tmpVector3;
			tmpVector3.x = mesh->mVertices[i].x;
			tmpVector3.y = mesh->mVertices[i].y;
			tmpVector3.z = mesh->mVertices[i].z;

			// - - - - - - Model Status - - - - - - // 
			sum_of_vertices.num_vertices++;
			sum_of_vertices.min_x = sum_of_vertices.min_x < tmpVector3.x ? sum_of_vertices.min_x : tmpVector3.x;
			sum_of_vertices.max_x = sum_of_vertices.max_x > tmpVector3.x ? sum_of_vertices.max_x : tmpVector3.x;
			sum_of_vertices.min_y = sum_of_vertices.min_y < tmpVector3.y ? sum_of_vertices.min_y : tmpVector3.y;
			sum_of_vertices.max_y = sum_of_vertices.max_y > tmpVector3.y ? sum_of_vertices.max_y : tmpVector3.y;
			sum_of_vertices.min_z = sum_of_vertices.min_z < tmpVector3.z ? sum_of_vertices.min_z : tmpVector3.z;
			sum_of_vertices.max_z = sum_of_vertices.max_z > tmpVector3.z ? sum_of_vertices.max_z : tmpVector3.z;
		}
	}

	void gettingNecessaryData(aiNode *node, const aiScene *scene)
	{

		for (size_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			gettingNecessaryData_from_mesh(mesh, scene);
		}

		for (size_t i = 0; i < node->mNumChildren; i++)
		{
			gettingNecessaryData(node->mChildren[i], scene);
		}
	}

	void processNode(aiNode *node, const aiScene *scene)
	{

		for (size_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.emplace_back(processMesh(mesh, scene));
		}
		for (size_t i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh *mesh, const aiScene *scene)
	{
		vector<Vertex> vertices;
		vector<GLuint> indices;
		vector<Texture> textures;

		double bbox_x = sum_of_vertices.max_x - sum_of_vertices.min_x;
		double bbox_y = sum_of_vertices.max_y - sum_of_vertices.min_y;
		double bbox_z = sum_of_vertices.max_z - sum_of_vertices.min_z;
		double max_size = bbox_x < bbox_y ? bbox_x : bbox_y;
		max_size = max_size < bbox_z ? max_size : bbox_z;


		double x_offset = (sum_of_vertices.max_x + sum_of_vertices.min_x) / 2.0;
		double y_offset = (sum_of_vertices.max_y + sum_of_vertices.min_y) / 2.0;
		double z_offset = (sum_of_vertices.max_z + sum_of_vertices.min_z) / 2.0;


		for (size_t i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 tmpVector3;
			tmpVector3.x = mesh->mVertices[i].x;
			tmpVector3.y = mesh->mVertices[i].y;
			tmpVector3.z = mesh->mVertices[i].z;
			vertex.Position = tmpVector3;

			vertex.Position.x -= x_offset;
			vertex.Position.y -= y_offset;
			vertex.Position.z -= z_offset;
			vertex.Position.x = vertex.Position.x / (max_size / 2.0);
			vertex.Position.y = vertex.Position.y / (max_size / 2.0);
			vertex.Position.z = vertex.Position.z / (max_size / 2.0);


			tmpVector3.x = mesh->mNormals[i].x;
			tmpVector3.y = mesh->mNormals[i].y;
			tmpVector3.z = mesh->mNormals[i].z;
			vertex.Normal = tmpVector3;

			if (mesh->mTextureCoords[0])
			{
				glm::vec2 tmpVector2;
				tmpVector2.x = mesh->mTextureCoords[0][i].x;
				tmpVector2.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoord = tmpVector2;
			}
			else
			{
				vertex.TexCoord = glm::vec2(0.0f);
			}

			vertices.emplace_back(vertex);
		}

		for (size_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (size_t j = 0; j < face.mNumIndices; j++)
			{
				indices.emplace_back(face.mIndices[j]);
			}
		}

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
			vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", materialIndex);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", materialIndex);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			vector<Texture> bumpMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_bump", materialIndex);
			textures.insert(textures.end(), bumpMaps.begin(), bumpMaps.end());
			materialIndex++;
		}
		return Mesh(vertices, indices, textures);
	}
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, size_t materialIndex)
	{
		vector<Texture> textures;
		for (size_t i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);

			bool skip = false;
			for (const auto elem : textures_loaded)
			{
				if (strcmp(str.C_Str(), elem.path.data()) == 0)
				{
					textures.push_back(elem);
					skip = true;
					break;
				}
			}

			if (!skip)
			{
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				texture.materialIndex = materialIndex;
				cout << typeName << " : " << texture.path << endl;

				aiColor4D color;
				aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &color);
				texture.colorP.ambient.x = color.r;
				texture.colorP.ambient.y = color.g;
				texture.colorP.ambient.z = color.b;

				aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color);
				texture.colorP.diffuse.x = color.r;
				texture.colorP.diffuse.y = color.g;
				texture.colorP.diffuse.z = color.b;

				aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color);
				texture.colorP.specular.x = color.r;
				texture.colorP.specular.y = color.g;
				texture.colorP.specular.z = color.b;

				float shine;
				aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &shine);
				texture.shiness = shine / 4.0f;

				textures.push_back(texture);
				textures_loaded.push_back(texture);


			}
		}
		return textures;
	}
};
