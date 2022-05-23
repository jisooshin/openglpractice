#include "util.hpp"

#ifdef DATA_PATH 
	#define PATH DATA_PATH 
#else
	#define PATH "undefined"
#endif
#ifdef SHADER_PATH 
	#define SPATH SHADER_PATH 
#else
	#define SPATH "undefined"
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
	string m_path(PATH);
	string s_path(SPATH);

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
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	/*
	Shader fbShader     (s_path + "/screen/main/ver.glsl",     s_path + "/screen/main/frag.glsl"    );
	Shader modelShader  (s_path + "/models/base/ver.glsl",     s_path + "/models/base/frag.glsl"    );
	Shader lightShader  (s_path + "/models/base/ver.glsl",     s_path + "/models/lights/frag.glsl"  );
	Shader outlineShader(s_path + "/effects/outline/ver.glsl", s_path + "/effects/outline/frag.glsl");
	Shader cubemapShader(s_path + "/background/ver.glsl", s_path + "/background/frag.glsl");

	fbShader.use();
	fbShader.setInt("screenTexture", 0);
	fbShader.setFloat("width_offset", 1.0f / WINDOW_WIDTH);
	fbShader.setFloat("height_offset", 1.0f / WINDOW_HEIGHT);


	CubeMap cubemap (m_path + "/background/skybox");
	Model model     (m_path + "/models/girl/girl.dae");
	Model outline   (m_path + "/models/girl/girl.dae");
	Light light(lightType::POINT, 1.0f, 1.0f, 0.09f, 0.032f);
	light.color = glm::vec3(1.0f, 1.0f, 1.0f);
	light.power = 4.0f;
	// Model lightball (m_path + "/models/circle/circle.obj");


	TM mMatrix, lMatrix, oMatrix;

	float scale_factor { 1.0f };
	glm::mat4 _model_model(1.0f);
	glm::vec3 model_location = glm::vec3(0.0f, 0.1f, 0.0f);
	float angle = glm::radians(-180.0f);
	glm::vec3 angle_vector = glm::vec3(1.0f, 0.0f, 0.0f);

	mMatrix.model = glm::rotate(mMatrix.model, angle, angle_vector);
	mMatrix.model = glm::scale(mMatrix.model, glm::vec3(scale_factor));
	mMatrix.model = glm::translate(mMatrix.model, model_location);

	oMatrix.model = glm::rotate(oMatrix.model, angle, angle_vector);
	oMatrix.model = glm::scale(oMatrix.model, glm::vec3(scale_factor));
	oMatrix.model = glm::translate(oMatrix.model, model_location);

	// lMatrix.model = glm::scale(lMatrix.model, glm::vec3(scale_factor * 0.3f));


	Screen screen((size_t)WINDOW_WIDTH, (size_t)WINDOW_HEIGHT);
	*/



	Model lightball (m_path + "/models/circle/circle.obj");

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	SphereMap map;
	GLuint vao, vbo, ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, map.base_vertices.size() * sizeof(Vertex), map.base_vertices.data(), GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, map.base_indices.size() * sizeof(GLuint), map.base_indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glBindVertexArray(0);

	Shader shader (s_path + "/models/icosahedron/ver.glsl",     s_path + "/models/icosahedron/frag.glsl");
	cout << " ============ " << endl;
	for (const auto elem: map.base_vertices)
	{
		cout << glm::to_string(elem.Normal) << endl;
	}
	cout << " ============ " << endl;

	int counter { 0 };
	while(!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		counter++;
		if (deltaTime >= 1.0 / 30.0)
		{
			string fps = to_string((1.0 / deltaTime) * counter);
			string new_title = "Test - " + fps + " FPS";
			glfwSetWindowTitle(window, new_title.c_str());
			lastFrame = currentFrame;
			counter = 0;
		}
		processInput(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		TM matrix;
		matrix.view = camera.GetViewMatrix();
		glm::mat4 projection(1.0f);
		projection = glm::perspective(
			glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
		matrix.projection = projection;
		matrix.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		glFrontFace(GL_CCW);
		shader.use();
		glm::vec3 position = glm::vec3(sin(glfwGetTime()), 0.0f, cos(glfwGetTime()));
		shader.setVec3("light_position", position);
		shader.setTransformMatrix("matrix", matrix);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, map.base_indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		/*	
		// view matrix and projection matrix
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection(1.0f);
		projection = glm::perspective(
			glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
		mMatrix.projection = projection;
		oMatrix.projection = projection;
		mMatrix.view = view;
		oMatrix.view = view;
		light.position = glm::vec3(0.0f, 0.0f, 6.0f);
		light.camera_position = camera.Position;
		lMatrix.model = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.2f)), light.position);
		screen.bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		modelShader.use();
		modelShader.setTransformMatrix("matrix", mMatrix);
		modelShader.setLight("point", light);
		model.Draw(modelShader);
		glStencilMask(0x00);
		glDepthFunc(GL_LEQUAL);
		cubemapShader.use();
		cubemapShader.setInt("skybox", 0);
		cubemapShader.setMat4("view", glm::mat4(glm::mat3(view)));
		cubemapShader.setMat4("projection", projection);
		cubemap.Draw(cubemapShader);
		glDepthFunc(GL_LESS);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
		outlineShader.use();
		outlineShader.setTransformMatrix("matrix", oMatrix);
		outlineShader.setFloat("outlineScale", 0.01f);
		outline.Draw(outlineShader);
		glEnable(GL_DEPTH_TEST);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glStencilMask(0xFF);
		screen.detach();
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT);
		screen.Draw(fbShader);
		glEnable(GL_DEPTH_TEST);
		*/
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

