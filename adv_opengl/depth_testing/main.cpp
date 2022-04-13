#include "util.hpp"

#define WINDOW_WIDTH 1300
#define WINDOW_HEIGHT 801

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
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
	glDepthFunc(GL_LESS);
	Shader _modelShader("../../shaders/models/ver.glsl", "../../shaders/models/frag.glsl");
	Shader _lightShader("../../shaders/lights/ver.glsl", "../../shaders/lights/frag.glsl");
	Model _backpack("../../data/sleeping_cat/sleeping_cat.obj");
	Model _lightball("../../data/circle/circle.obj");


	// - - - - - ↓ ↓ ↓ ↓ - - - - - // 
	float planeVertices[] = {
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f								
    };
	GLuint planeVAO, planeVBO;
	GLuint planeTextureID = TextureFromFile("floor.jpg", "../../image");
	Shader _planeShader("../../shaders/plane/ver.glsl", "../../shaders/plane/frag.glsl");
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);

	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);
	// - - - - - ↑ ↑ ↑ ↑ - - - - - // 



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

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection(1.0f);
		projection = glm::perspective(
			glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

		glm::mat4 _model_view = camera.GetViewMatrix();
		glm::mat4 _model_model(1.0f);
		_model_model = glm::translate(_model_model, glm::vec3(0.0f, 0.0f, 0.0f));
		_model_model = glm::rotate(_model_model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		_model_model = glm::scale(_model_model, glm::vec3(1.0));

		glm::mat4 _light_model(1.0f);
		glm::vec3 _light_position = glm::vec3(sin(glfwGetTime()) * 3.0f, 0.0f, cos(glfwGetTime()) * 3.0f);
		_light_model = glm::translate(_light_model, _light_position);
		_light_model = glm::scale(_light_model, glm::vec3(0.2f));


		_modelShader.use();
		_modelShader.setMat4("view", _model_view);
		_modelShader.setMat4("projection", projection);
		_modelShader.setMat4("model", _model_model);

		_modelShader.setVec3 ("point.lv_Position", _light_position);
		_modelShader.setFloat("point.lf_Constant", 1.0f);
		_modelShader.setFloat("point.lf_LinearParam", 0.09f);
		_modelShader.setFloat("point.lf_QuadParam", 0.032f);
		_modelShader.setFloat("point.lf_Power", 2.0f);
		_modelShader.setVec3 ("point.lv_CameraPosition", camera.Position);

		_backpack.Draw(_modelShader);


		_lightShader.use();
		_lightShader.setMat4("view", _model_view);
		_lightShader.setMat4("projection", projection);
		_lightShader.setMat4("model", _light_model);
		_lightball.Draw(_lightShader);


		// - - - - - ↓ ↓ ↓ ↓ - - - - - // 
		glBindVertexArray(planeVAO);
		_planeShader.use();
		glm::mat4 _plane_model = glm::mat4(1.0f);
		_plane_model = glm::translate(_plane_model, glm::vec3(0.0f, 0.0f, 0.0f));
		_plane_model = glm::scale(_plane_model, glm::vec3(2.0f, 2.0f, 2.0f));
		_planeShader.setMat4("model", _plane_model);
		_planeShader.setMat4("view", _model_view);
		_planeShader.setMat4("projection", projection);
		_planeShader.setInt("floorTexture", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, planeTextureID);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// - - - - - ↑ ↑ ↑ ↑ - - - - - // 

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

