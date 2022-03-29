#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include "util.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define WINDOW_WIDTH 1300
#define WINDOW_HEIGHT 800

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void captureImage(std::string&& file_path, GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
GLuint loadTexture(const char* imagePath);
string shader_uniform_formatter(string&& fmt, int idx);



Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
float deltaTime  = 0.0f;
float lastFrame  = 0.0f;
bool  firstMouse = true;
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = 
		glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Testing", NULL, NULL);

	if (window == NULL)
	{
		printf("Failed to create GLFW window\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to initialize GLAD\n");
		return -1;
	}
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// global
	glEnable(GL_DEPTH_TEST);


	float vertices[288];
	ifstream in("data/vertices_288.dat", ios::in | ios::binary);
	in.read((char*)&vertices, sizeof(vertices));


	glm::vec3 cubePositions[] = {
		glm::vec3( 0.0f,  0.0f,  0.0f),
		glm::vec3( 2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3( 2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3( 1.3f, -2.0f, -2.5f),
		glm::vec3( 1.5f,  2.0f, -2.5f),
		glm::vec3( 1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	glm::vec3 pointLightPositions[] = {
		glm::vec3( 0.7f,  0.2f,  2.0f),
		glm::vec3( 2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3( 0.0f,  0.0f, -3.0f)
	};  

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);

	// glBindVertexArray(0);
	// glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint lampVAO;
	glGenVertexArrays(1, &lampVAO);

	glBindVertexArray(lampVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// glBindVertexArray(0);
	// glBindBuffer(GL_ARRAY_BUFFER, 0);

	// -> texture 
	GLuint diffuseMap  = loadTexture("image/container2.png");
	GLuint specularMap = loadTexture("image/container2_specular.png");
	GLuint lightTextureMap = loadTexture("image/flashlight_texture.jpg");

	Shader cubeShader ("shaders/cube_v.glsl", "shaders/cube_f.glsl");
	Shader lampShader ("shaders/lamp_v.glsl", "shaders/lamp_f.glsl");

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);



	cubeShader.use();
	// texture binding
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);

	cubeShader.setInt("material.diffuse", 0);
	cubeShader.setInt("material.specular", 1);

	while(!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		captureImage("saved_image/image.png", window);

		glClearColor(0.05f, 0.08f, 0.07f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cubeShader.use();
		cubeShader.setVec3("viewPos", camera.Position);
		cubeShader.setFloat("material.shininess", 32.0f);
		
		// 1 directional light
		float iParam = 0.8f;
		cubeShader.setVec3("directionalLight.direction", glm::vec3(1.0f, 1.0f, -1.0f));
		cubeShader.setVec3("directionalLight.ambient",  glm::vec3(0.2f * iParam));
		cubeShader.setVec3("directionalLight.diffuse",  glm::vec3(0.5f * iParam));
		cubeShader.setVec3("directionalLight.specular", glm::vec3(1.0f * iParam));

		// 4 point lights
		for (int i = 0; i < 4; i++)
		{
			glm::vec3 pointLight_position = pointLightPositions[i];
			cubeShader.setVec3( shader_uniform_formatter("pointLights[%i].position", i), pointLight_position);
			cubeShader.setFloat(shader_uniform_formatter("pointLights[%i].constant", i), 1.0f);
			cubeShader.setFloat(shader_uniform_formatter("pointLights[%i].linearParam", i), 0.09f);
			cubeShader.setFloat(shader_uniform_formatter("pointLights[%i].quadraticParam", i), 0.032f);
			cubeShader.setVec3( shader_uniform_formatter("pointLights[%i].ambient", i),  glm::vec3(0.2f * iParam));
			cubeShader.setVec3( shader_uniform_formatter("pointLights[%i].diffuse", i),  glm::vec3(0.5f * iParam));
			cubeShader.setVec3( shader_uniform_formatter("pointLights[%i].specular", i), glm::vec3(1.0f * iParam));
		}

		glm::mat4 projection  = glm::mat4(1.0f);
		projection = glm::perspective(
			glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

		glBindVertexArray(VAO);
		for (int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i * glfwGetTime();
			model = glm::rotate(model, glm::radians(angle), glm::vec3(0.5f, 1.0f, 0.2f));
			cubeShader.setMat4("projection", projection);
			cubeShader.setMat4("view", camera.GetViewMatrix());
			cubeShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		lampShader.use();
		glBindVertexArray(lampVAO);
		for(int i = 0; i < 4; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.3f));
			lampShader.setMat4("projection", projection);
			lampShader.setMat4("view", camera.GetViewMatrix());
			lampShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lampVAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
}

void captureImage(std::string&& file_path, GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		int nrChannels = 3;
		int stride = nrChannels * width;
		stride += (stride % 4) ? (4 - stride % 4) : 0;
		int bufferSize = stride * height;
		std::vector<char> buffer(bufferSize);
		glPixelStorei(GL_PACK_ALIGNMENT, 4);
		glReadBuffer(GL_FRONT);
		glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
		stbi_flip_vertically_on_write(true);

		stbi_write_png(file_path.c_str(), width, height, nrChannels, buffer.data(), stride);
	}
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		firstMouse = false;
		lastX = xpos;
		lastY = ypos;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll((float)yoffset);
}


GLuint loadTexture(const char* imagePath)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height, channels;
	unsigned char* data = stbi_load(imagePath, &width, &height, &channels, 0);
	if (data)
	{
		GLenum format;
        if (channels == 1)
            format = GL_RED;
        else if (channels == 3)
            format = GL_RGB;
        else if (channels == 4)
            format = GL_RGBA;
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else
	{
		throw "Failed to read Image file.\n";
	}
	stbi_image_free(data);
	return textureID;

}


string shader_uniform_formatter(string&& fmt, int idx)
{
	
	int sz = snprintf(nullptr, 0, fmt.c_str(), idx);
	char buf[sz + 1];
	snprintf(buf, sizeof(buf), fmt.c_str(), idx);
	string baseStr(buf);
	return baseStr;
}
