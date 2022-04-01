#pragma once
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


const float YAW         = -90.0f;
const float PITCH       = 0.0f;
const float SPEED       = 2.5f;
const float SENSITIVITY = 0.05f;
const float ZOOM        = 45.0f;

enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

using namespace std;
class Shader
{
	public:
		GLuint ID;
		Shader(const char* vertexPath, const char* fragPath);
		void use();
		void setBool  (const string& name, bool value)      const;
		void setInt   (const string& name, int value)       const;
		void setFloat (const string& name, float value)     const;
		void setMat4  (const string& name, glm::mat4 value) const;
		void setVec3  (const string& name, glm::vec3 v)     const;
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
			glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f),
			float yaw = YAW,
			float pitch = PITCH
		) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
			MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
		{
			Position = position;
			WorldUp  = up; Yaw      = yaw;
			Pitch    = pitch;
			updateCameraVectors();
		}
		Camera(
			float posX, float posY, float posZ,
			float upX, float upY, float upZ,
			float yaw, float pitch
		) : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
			MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
		{
			Position = glm::vec3(posX, posY, posZ);
			WorldUp  = glm::vec3(upX, upY, upZ);
			Yaw      = yaw;
			Pitch    = pitch;
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
			Yaw     += xoffset;
			Pitch   += yoffset;

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
        	Up    = glm::normalize(glm::cross(Right, Front));
		}
};

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 TexCoord;
};

struct Texture
{
	GLuint id;
	string type; // specular 인지 diffuse 인지 등을 저장하는 type
};

class Mesh
{
	public:
		vector<Vertex>  vertices;
		vector<GLuint>  indices;
		vector<Texture> textures;
		Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
			: vertices(vertices), indices(indices), textures(textures){setupMesh();};
		void Draw(Shader &shader)
		{
			GLuint diffuseN  = 1;
			GLuint specularN = 1;
			for (size_t i = 0; i < textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i);

				string number;
				string name = textures[i].type;
				if (name == "texture_diffuse")
					number = to_string(diffuseN++);
				else if (name == "texture_specular")
					number = to_string(specularN++);
				shader.setFloat(("material." + name + number).c_str(), i);
				glBindTexture(GL_TEXTURE_2D, textures[i].id);
			}
			glActiveTexture(GL_TEXTURE0);

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
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
			
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));
			glBindVertexArray(0);
		}
};


class Model 
{
	public:
		Model(char* path)
		{
			loadModel(path);
		}
		void Draw(Shader& shader)
		{
			for (auto mesh: meshes)
			{
				mesh.Draw(shader);
			}
		}
	private:
		vector<Mesh> meshes;
		string directory;
		void loadModel(string path){
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		}
		void processNode(aiNode* node, const aiScene* scene);
		void processMesh(aiMesh* mesh, const aiScene* scene);
		vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};