#pragma once
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>


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
};