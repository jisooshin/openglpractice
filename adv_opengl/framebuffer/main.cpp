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


    float screenVerticies[] = { 
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f
    };

	// - - - - - - - - - - - - - - - - - - - - // 
	GLuint screenVAO, screenVBO;
	glGenVertexArrays(1, &screenVAO);
	glGenBuffers(1, &screenVBO);
	glBindVertexArray(screenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenVerticies), &screenVerticies, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);


	GLuint frameBuffer, fColorBuffer, fRenderBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	// color 버퍼 붙이기
	glGenTextures(1, &fColorBuffer);
	glBindTexture(GL_TEXTURE_2D, fColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fColorBuffer, 0);

	// render 버퍼 붙이기
	glGenRenderbuffers(1, &fRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, fRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fRenderBuffer);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		printf("FRAMEBUFFER BIND SUCCESSFULLY.\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// - - - - - - - - - - - - - - - - - - - - // 


	// Globally // 
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);



	Shader modelShader("../../shaders/models/ver.glsl", "../../shaders/models/frag.glsl");
	Shader outlineShader("../../shaders/outline/ver.glsl", "../../shaders/outline/frag.glsl");
	Shader lightShader("../../shaders/models/ver.glsl", "../../shaders/lights/frag.glsl");
	Shader fbShader("../../shaders/framebuffer/ver.glsl", "../../shaders/framebuffer/frag.glsl");
	fbShader.use();
	fbShader.setInt("screenTexture", 0);
	fbShader.setFloat("width_offset", 1.0f / WINDOW_WIDTH);
	fbShader.setFloat("height_offset", 1.0f / WINDOW_HEIGHT);

	Light light(lightType::POINT, 1.0f, 1.0f, 0.09f, 0.032f);
	light.color = glm::vec3(1.0f, 1.0f, 1.0f);
	light.power = 8.0f;


	Model model     (path + "/girl/girl.dae");
	Model outline   (path + "/girl/girl.dae");
	Model lightball (path + "/circle/circle.obj");

	TM mMatrix, lMatrix, oMatrix;

	float scale_factor { 1.0f };
	glm::mat4 _model_model(1.0f);
	glm::vec3 model_location = glm::vec3(0.0f, 0.5f, 0.0f);
	float angle = glm::radians(-180.0f);
	glm::vec3 angle_vector = glm::vec3(1.0f, 0.0f, 0.0f);

	mMatrix.model = glm::translate(glm::mat4(1.0f), model_location);
	mMatrix.model = glm::rotate(mMatrix.model, angle, angle_vector);
	mMatrix.model = glm::scale(mMatrix.model, glm::vec3(scale_factor));

	oMatrix.model = glm::translate(glm::mat4(1.0f), model_location);
	oMatrix.model = glm::rotate(oMatrix.model, angle, angle_vector);
	oMatrix.model = glm::scale(oMatrix.model, glm::vec3(scale_factor));

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
		
		// view matrix and projection matrix
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection(1.0f);
		projection = glm::perspective(
			glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
		
		mMatrix.projection = projection;
		oMatrix.projection = projection;
		lMatrix.projection = projection;

		mMatrix.view = view;
		oMatrix.view = view;
		lMatrix.view = view;

		// light.position = glm::vec3(sin(glfwGetTime()) * 10.0f, 5.0f, cos(glfwGetTime()) * 10.0f);
		light.position = glm::vec3(0.0f, 0.0f, 10.0f);
		light.camera_position = camera.Position;
		lMatrix.model = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.2f)), light.position);

		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		glEnable(GL_DEPTH_TEST);
		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// 잠깐 Lightball은 없애는걸로
		glStencilMask(0x00);
		glFrontFace(GL_CW);
		lightShader.use();
		lightShader.setTransformMatrix("matrix", lMatrix);
		lightShader.setVec3("color", light.color);
		lightball.Draw(lightShader);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		glFrontFace(GL_CCW);
		modelShader.use();
		modelShader.setTransformMatrix("matrix", mMatrix);
		modelShader.setLight("point", light);
		model.Draw(modelShader);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
		glFrontFace(GL_CCW);
		outlineShader.use();
		outlineShader.setTransformMatrix("matrix", oMatrix);
		outlineShader.setFloat("outlineScale", 0.003f);
		outline.Draw(outlineShader);

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glDisable(GL_DEPTH_TEST);
		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(screenVAO);
		fbShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fColorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);


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

