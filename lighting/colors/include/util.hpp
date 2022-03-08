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
		void setVec3(const string& name, float v1, float v2, float v3) const;
};

class Camera
// 애초에 카메라 view 매트릭스를 만드는 것이 목적인 class
// rendering loop 안에서 
{
	public:
		Camera(glm::vec3 cameraPos, glm::vec3 cameraDirection, glm::vec3 cameraUp);
		void moveKeyboard_Forward (); // 항상 projection * view 의 결과 matrix를 뱉어내도록 한다.
		void moveKeyboard_Backward();
		void moveKeyboard_Left    ();
		void moveKeyboard_Right   ();
		void moveMouse            (double xpos, double ypos, bool isFirstMouse);
		void scale_up             ();
		void scale_down           (); 
	private:
		glm::mat4 cameraMatrix = glm::mat4(1.0f);
		float yaw   = -90.0f;
		float pitch =   0.0f;
		float fov   =  45.0f;
};
