#include "util.hpp"

#define WINDOW_WIDTH 1600	
#define WINDOW_HEIGHT 800
#define PROJ_NEAR    0.1f
#define PROJ_FAR   100.0f

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
	glEnable(GL_STENCIL_TEST);
	// default mask = 0x00
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	// glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	// glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	// glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


	Shader _modelShader("../../shaders/models/ver.glsl", "../../shaders/models/frag.glsl");
	Shader _lightShader("../../shaders/lights/ver.glsl", "../../shaders/lights/frag.glsl");
	Shader _outlineShader("../../shaders/models/outline/ver.glsl", "../../shaders/models/outline/frag.glsl");

	Model _model1("../../data/tiber/Tibbers Hextech.obj");
	Model _model2("../../data/sleeping_cat/sleeping_cat.obj");
	Model _model3("../../data/backpack/backpack.obj");

	Model _lightball("../../data/circle/circle.obj");

	Model modelContaienr[3] = { _model1, _model2, _model3 };
	float modelParamContainer[3][4] = 
	{
		{1.0f, 0.0f, 0.1f, 0.0f},
		{0.3f, 2.0f, -0.75f, 2.0f},
		{0.8f, -1.0f, 0.0f, 0.5f}
	};


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

		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glm::mat4 projection(1.0f);
		glm::mat4 cameraView = camera.GetViewMatrix();
		projection = glm::perspective(
			glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, PROJ_NEAR, PROJ_FAR);


		glm::mat4 _light_model(1.0f);
		glm::vec3 _light_position = glm::vec3(sin(glfwGetTime() * 2.0f) * 3.0f, 0.0f, cos(glfwGetTime() * 2.0f) * 3.0f);
		_light_model = glm::translate(_light_model, _light_position);
		_light_model = glm::scale(_light_model, glm::vec3(0.2f));


		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		for (int i = 0; i < 3; i++)
		{
			glm::mat4 model(1.0f);
			model = glm::scale(model, glm::vec3(modelParamContainer[i][0]));
			model = glm::translate(model, glm::vec3(
				modelParamContainer[i][1],
				modelParamContainer[i][2],
				modelParamContainer[i][3]));

			if (i == 1)
			{
				model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			_modelShader.use();
			_modelShader.setMat4("view", cameraView);
			_modelShader.setMat4("projection", projection);
			_modelShader.setMat4("model", model);
			_modelShader.setVec3 ("point.lv_Position", _light_position);
			_modelShader.setFloat("point.lf_Constant", 1.0f);
			_modelShader.setFloat("point.lf_LinearParam", 0.09f);
			_modelShader.setFloat("point.lf_QuadParam", 0.032f);
			_modelShader.setFloat("point.lf_Power", 2.0f);
			_modelShader.setVec3 ("point.lv_CameraPosition", camera.Position);
			modelContaienr[i].Draw(_modelShader);

			// glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			// glStencilMask(0x00);
			// model = glm::scale(model, glm::vec3(1.02f));
			// _outlineShader.use();
			// _outlineShader.setMat4("view", cameraView);
			// _outlineShader.setMat4("projection", projection);
			// _outlineShader.setMat4("model", model);
			// _outlineShader.setVec3 ("point.lv_Position", _light_position);
			// _outlineShader.setFloat("point.lf_Constant", 1.0f);
			// _outlineShader.setFloat("point.lf_LinearParam", 0.09f);
			// _outlineShader.setFloat("point.lf_QuadParam", 0.032f);
			// _outlineShader.setFloat("point.lf_Power", 2.0f);
			// _outlineShader.setVec3 ("point.lv_CameraPosition", camera.Position);
			// modelContaienr[i].Draw(_outlineShader);
		}


		_lightShader.use();
		_lightShader.setMat4("view", cameraView);
		_lightShader.setMat4("projection", projection);
		_lightShader.setMat4("model", _light_model);
		_lightball.Draw(_lightShader);


		// - - - - - ↓ ↓ ↓ ↓ - - - - - // 
		glStencilMask(0x00);
		glBindVertexArray(planeVAO);
		_planeShader.use();
		glm::mat4 _plane_model = glm::mat4(1.0f);
		_plane_model = glm::translate(_plane_model, glm::vec3(0.0f, 0.0f, 0.0f));
		_plane_model = glm::scale(_plane_model, glm::vec3(2.0f, 2.0f, 2.0f));
		_planeShader.setMat4("model", _plane_model);
		_planeShader.setMat4("view", cameraView);
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

