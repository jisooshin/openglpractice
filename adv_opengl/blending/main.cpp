#include "util.hpp"
#include <filesystem>

#ifdef DATA_PATH 
	#define PATH DATA_PATH 
#else
	#define PATH "undefined"
#endif

#define WINDOW_WIDTH 1300
#define WINDOW_HEIGHT 801

typedef CollectionOfTransformMatrix TM;

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
	string path(PATH);

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
	// Globally // 
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glDepthFunc(GL_LESS);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	Light light(lightType::POINT, 1.0f, 1.0f, 0.09f, 0.032f);
	light.color = glm::vec3(0.9f, 0.9f, 1.0f);
	light.power = 20.0f;

	Shader modelShader("../../shaders/models/ver.glsl", "../../shaders/models/frag.glsl");
	Shader outlineShader("../../shaders/outline/ver.glsl", "../../shaders/outline/frag.glsl");
	Shader lightShader("../../shaders/models/ver.glsl", "../../shaders/lights/frag.glsl");
	Shader floorShader("../../shaders/models/ver.glsl", "../../shaders/plane/frag.glsl");

	Model model    (path + "/gun/Handgun_dae.dae");
	Model outline  (path + "/gun/Handgun_dae.dae");
	Model lightball(path + "/circle/circle.obj");
	Model floor    (path + "/floor/floor.dae");

	TM mMatrix, lMatrix, fMatrix, oMatrix;
	float scale_factor { 0.2f };
	glm::mat4 _model_model(1.0f);
	glm::vec3 model_location = glm::vec3(0.0f, 0.5f, 0.0f);

	mMatrix.model = glm::translate(glm::mat4(1.0f), model_location);
	mMatrix.model = glm::rotate(mMatrix.model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mMatrix.model = glm::scale(mMatrix.model, glm::vec3(scale_factor));

	oMatrix.model = glm::translate(glm::mat4(1.0f), model_location);
	oMatrix.model = glm::rotate(oMatrix.model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	oMatrix.model = glm::scale(oMatrix.model, glm::vec3(scale_factor));


	fMatrix.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f));
	fMatrix.model = glm::rotate(fMatrix.model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	fMatrix.model = glm::scale(fMatrix.model, glm::vec3(0.1f));
	// ---------------------- //


	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	while(!glfwWindowShouldClose(window))
	{
		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		
		// view matrix and projection matrix
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection(1.0f);
		projection = glm::perspective(
			glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

		mMatrix.projection = projection;
		lMatrix.projection = projection;
		fMatrix.projection = projection;
		oMatrix.projection = projection;
		mMatrix.view = view;
		lMatrix.view = view;
		fMatrix.view = view;
		oMatrix.view = view;

		light.position = glm::vec3(sin(glfwGetTime()) * 20.0f, 5.0f, cos(glfwGetTime()) * 20.0f);
		light.camera_position = camera.Position;
		lMatrix.model = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.2f)), light.position);

		// plane //
		glStencilMask(0x00);
		floorShader.use();
		floorShader.setTransformMatrix("matrix", fMatrix);
		floorShader.setLight("point", light);
		floor.Draw(floorShader);

		// -- light -- //
		glStencilMask(0x00);
		lightShader.use();
		lightShader.setTransformMatrix("matrix", lMatrix);
		lightShader.setVec3("color", light.color);
		lightball.Draw(lightShader);
		// ----------- //

		// -- model -- // 
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		modelShader.use();
		modelShader.setTransformMatrix("matrix", mMatrix);
		modelShader.setLight("point", light);
		model.Draw(modelShader);

		// -- outline -- // 
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
		outlineShader.use();
		outlineShader.setTransformMatrix("matrix", oMatrix);
		outlineShader.setFloat("outlineScale", 0.05f);
		outline.Draw(outlineShader);
		glEnable(GL_DEPTH_TEST);


		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glStencilMask(0xFF);

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

