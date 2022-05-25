#pragma once
#include <filesystem>
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
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>




using namespace std;
enum lightType { POINT, SPOT, DIRECTIONAL };
enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

struct Light
{
	lightType type;
	float power;
	float att_constant, att_linear, att_quad;
	glm::vec3 color;
	glm::vec3 position;
	glm::vec3 camera_position;

	Light(
		lightType type,
		float power,
		float att_constant,
		float att_linear,
		float att_quad, 
		glm::vec3 color = glm::vec3(1.0f),
		glm::vec3 position = glm::vec3(0.0f),
		glm::vec3 camera_position = glm::vec3(0.0f))
	: type(type), power(power), att_constant(att_constant), att_linear(att_linear), att_quad(att_quad), color(color), position(position), camera_position(camera_position)
	{}
};

struct CollectionOfTransformMatrix
{
	glm::mat4 view, projection, model;
	CollectionOfTransformMatrix(
		glm::mat4 model = glm::mat4(1.0f),
		glm::mat4 view = glm::mat4(1.0f),
		glm::mat4 projection = glm::mat4(1.0f))
	: model(model), view(view), projection(projection){}
};

struct Vertex
{
	glm::vec3 Position = glm::vec3(0.0f); // vertex 좌표
	glm::vec3 Normal = glm::vec3(0.0f);	// vertex 법선벡터
	glm::vec2 TexCoord = glm::vec3(0.0f); // (만약 있다면) 해당 vertex가 Texture 이미지의 어느 좌표에 속하는지에 대한 좌표
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
	string node_name;
	string type; // specular 인지 diffuse 인지 등을 저장하는 type
	string path; // filePath (Texture를 나타내는 이미지 파일이 따로 있는 경우를 나타내기 위함인듯 )
	size_t materialIndex = 0;
	ColorProperty colorP;
	float shiness;
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


// - - - - - - - - - - ↓ ↓ ↓ ↓ ↓ ↓ - - - - - - - - - - // 
//       Function and Class Declarations               //  
// - - - - - - - - - - ↓ ↓ ↓ ↓ ↓ ↓ - - - - - - - - - - // 

GLuint TextureFromFile(const char *path, const string &directory);
GLuint LoadCubeMap(vector<string> faces);
string format_stringi(string &&fmt, int idx);
glm::vec3 cross_product(glm::vec3 a, glm::vec3 b);

class Shader
{
	public:
		GLuint ID;
		Shader(string vShader, string fShader);
		void use();
		void setBool(const string &name, bool value) const;
		void setInt(const string &name, int value) const;
		void setFloat(const string &name, float value) const;
		void setMat4(const string &name, glm::mat4 value) const;
		void setVec3(const string &name, glm::vec3 v) const;
		void setLight(const string& name, Light light) const;
		void setTransformMatrix(const string& name, CollectionOfTransformMatrix matrix) const;
};



// Camera default parameters
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.05f;
const float ZOOM = 45.0f;

class Camera
{
	public:
		glm::vec3 Position, Front, Up, Right, WorldUp;
		float Yaw, Pitch, MovementSpeed, MouseSensitivity, Zoom;
		Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
		Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
		glm::mat4 GetViewMatrix();
		void ProcessKeyboard(Camera_Movement direction, float deltaTime);
		void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
		void ProcessMouseScroll(float yoffset);
	private:
		void updateCameraVectors();
};


class Mesh
{
	public:
		vector<Vertex> vertices;
		vector<GLuint> indices;
		vector<Texture> textures;
		Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);
		void Draw(Shader& shader);
	private:
		GLuint VAO, VBO, EBO;
		void setupMesh();
};


class Model
// 여러 mesh들이 모여 model이 된다
{
	public:
		Model(string path); // 이 path 는 model을 담아두는 파일이 될 것
		void Draw(Shader &shader);
	private:
		sumCoord sum_of_vertices;
		vector<Mesh> meshes;
		vector<Texture> textures_loaded;
		string directory;
		size_t materialIndex = 0;

		void loadModel(string path);
		void gettingNecessaryData_from_mesh(aiMesh *mesh, const aiScene *scene);
		void gettingNecessaryData(aiNode *node, const aiScene *scene);
		void processNode(aiNode *node, const aiScene *scene);
		Mesh processMesh(aiMesh *mesh, const aiScene *scene, const glm::mat4 transformMat, string nodeName);
		vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, size_t materialIndex);
};


class Screen
{
	public:
		Screen(size_t width, size_t height);
		void Draw(Shader& shader);
		void bind();
		void detach();

		size_t width, height;
		GLuint id;
		GLuint vao, vbo;
		GLuint color_buffer, render_buffer;
	private:
		void set();
		
};


class CubeMap
{
	public:
		CubeMap(string dir_path);
		void Draw(Shader& shader);
	private:
		GLuint texture;
		GLuint vao, vbo;
		void set(string path);

};

class SphereMap
{
	public:
		SphereMap();
		vector<Vertex> base_vertices;
		vector<GLuint> base_indices;
	private:
		vector<GLuint> indices;
		vector<Vertex> vertices;
		void build_base();
		void build_normal();
		void expand_and_texturing(int width, int height);
		void divide_icosahedron(int subdivision);
		Vertex get_half_vertex(Vertex v1, Vertex v2);
};
