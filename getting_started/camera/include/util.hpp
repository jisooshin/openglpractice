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


using namespace std;
class Shader
{
	public:
		GLuint ID;
		Shader(const char* vertexPath, const char* fragPath);
		void use();
		void setBool(const string& name, bool value) const;
		void setInt(const string& name, int value) const;
		void setFloat(const string& name, float value) const;
		void setMat4(const string& name, glm::mat4 value) const;
};

class Camera
{
	public:
		Camera();
		void move_Forward();
		void move_Backward();
		void move_();

		
};
