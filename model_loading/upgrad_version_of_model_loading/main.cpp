#include "util.hpp"

#define WINDOW_WIDTH 1300
#define WINDOW_HEIGHT 801

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
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
	Shader _modelShader("../../shaders/models/ver.glsl", "../../shaders/models/frag.glsl");
	Shader _lightShader("../../shaders/lights/ver.glsl", "../../shaders/lights/frag.glsl");
	Model _backpack("../../data/backpack/backpack.obj");
	Model _lightball("../../data/circle/circle.obj");

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	while(!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.0f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection(1.0f);
		projection = glm::perspective(
			glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

		glm::mat4 _model_view = camera.GetViewMatrix();
		glm::mat4 _model_model(1.0f);
		_model_model = glm::translate(_model_model, glm::vec3(0.0f, 0.0f, 0.0f));
		_model_model = glm::scale(_model_model, glm::vec3(1.0));

		glm::mat4 _light_model(1.0f);
		glm::vec3 _light_position = glm::vec3(sin(glfwGetTime()) * 2.0f, 2.0f, cos(glfwGetTime()) * 2.0f);
		_light_model = glm::translate(_light_model, _light_position);
		_light_model = glm::scale(_light_model, glm::vec3(0.2f));


		_modelShader.use();
		_modelShader.setMat4("view", _model_view);
		_modelShader.setMat4("projection", projection);
		_modelShader.setMat4("model", _model_model);

		_modelShader.setVec3("point.position", _light_position);
		_modelShader.setFloat("point.constant", 1.0f);
		_modelShader.setFloat("point.linearParam", 0.09f);
		_modelShader.setFloat("point.quadParam", 0.032f);
		_modelShader.setVec3("point.cameraPosition", camera.Position);
		_modelShader.setFloat("point.shiness", 64.0f);

		_backpack.Draw(_modelShader);


		_lightShader.use();
		_lightShader.setMat4("view", _model_view);
		_lightShader.setMat4("projection", projection);
		_lightShader.setMat4("model", _light_model);
		_lightball.Draw(_lightShader);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

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

