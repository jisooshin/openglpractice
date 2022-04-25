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

	Light light(lightType::POINT, 1.0f, 1.0f, 0.09f, 0.032f);

	Shader modelShader("../../shaders/models/ver.glsl", "../../shaders/models/frag.glsl");
	Shader lightShader("../../shaders/lights/ver.glsl", "../../shaders/lights/frag.glsl");

	Model model(path + "/gun/Handgun_dae.dae");
	Model lightball(path + "/circle/circle.obj");
	TM mMatrix, lMatrix;
	
	// --> process plane <-- //
	float planeVertices[] = {
        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
        5.0f, -0.5f, -5.0f,  2.0f, 2.0f								
    };
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
	glBindVertexArray(0);
	Shader planeShader("../../shaders/plane/ver.glsl", "../../shaders/plane/frag.glsl");
	GLuint planeTextureId = TextureFromFile("white.jpg", "../../image");
	planeShader.setInt("planeTexture", 0);
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

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection(1.0f);
		projection = glm::perspective(
			glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

		mMatrix.projection = projection;
		lMatrix.projection = projection;
		mMatrix.view = view;
		lMatrix.view = view;

		// glm::mat4 _light_model(1.0f);
		light.position = glm::vec3(sin(glfwGetTime()) * 10.0f, 2.0f, cos(glfwGetTime()) * 10.0f);
		lMatrix.model = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.2f)), light.position);

		// -- plane -- // 
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, planeTextureId);
		glm::mat4 _plane_model(1.0f);
		planeShader.use();
		planeShader.setMat4("view", view);
		planeShader.setMat4("projection", projection);
		planeShader.setMat4("model", _plane_model);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		// ----------- //
		// -- model -- // 
		glm::mat4 _model_model(1.0f);
		_model_model = glm::translate(_model_model, glm::vec3(0.0f, 0.5f, 0.0f));
		_model_model = glm::rotate(_model_model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		_model_model = glm::scale(_model_model, glm::vec3(0.2f));

		modelShader.use();
		modelShader.setMat4("view", mMatrix.view);
		modelShader.setMat4("projection", mMatrix.projection);
		modelShader.setMat4("model", _model_model);
		modelShader.setVec3 ("point.lv_Position", light.position);
		modelShader.setFloat("point.lf_Constant", 1.0f);
		modelShader.setFloat("point.lf_LinearParam", 0.09f);
		modelShader.setFloat("point.lf_QuadParam", 0.032f);
		modelShader.setFloat("point.lf_Power", 2.0f);
		modelShader.setVec3 ("point.lv_CameraPosition", camera.Position);
		model.Draw(modelShader);

		// ----------- //
		// -- outline -- // 
		// glm::mat4 _outline_model(glm::scale(_model_model, glm::vec3(1.08f)));
		// _outline.use();
		// _outline.setMat4("view", view);
		// _outline.setMat4("projection", projection);
		// _outline.setMat4("model", _outline_model);
		// _outline.setVec3 ("point.lv_Position", _light_position);
		// _outline.setFloat("point.lf_Constant", 1.0f);
		// _outline.setFloat("point.lf_LinearParam", 0.09f);
		// _outline.setFloat("point.lf_QuadParam", 0.032f);
		// _outline.setFloat("point.lf_Power", 2.0f);
		// _outline.setVec3 ("point.lv_CameraPosition", camera.Position);
		// _model.Draw(_outline);
		// ----------- //


		// -- light -- //
		lightShader.use();
		lightShader.setMat4("view", lMatrix.view);
		lightShader.setMat4("projection", lMatrix.projection);
		lightShader.setMat4("model", lMatrix.model);
		lightball.Draw(lightShader);
		// ----------- //

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

