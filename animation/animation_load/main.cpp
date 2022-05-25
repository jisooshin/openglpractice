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

	CubeMap cubemap(m_path + "/background/skybox");


	// Globally // 
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	Shader fbShader     (s_path + "/screen/main/ver.glsl",     s_path + "/screen/main/frag.glsl"    );
	Shader modelShader  (s_path + "/models/base/ver.glsl",     s_path + "/models/base/frag.glsl"    );
	Shader lightShader  (s_path + "/models/base/ver.glsl",     s_path + "/models/lights/frag.glsl"  );
	Shader outlineShader(s_path + "/effects/outline/ver.glsl", s_path + "/effects/outline/frag.glsl");
	Shader cubemapShader(s_path + "/background/ver.glsl", s_path + "/background/frag.glsl");

	fbShader.use();
	fbShader.setInt("screenTexture", 0);
	fbShader.setFloat("width_offset", 1.0f / WINDOW_WIDTH);
	fbShader.setFloat("height_offset", 1.0f / WINDOW_HEIGHT);

	Light light(lightType::POINT, 1.0f, 1.0f, 0.09f, 0.032f);
	light.color = glm::vec3(1.0f, 1.0f, 1.0f);
	light.power = 5.0f;

	// Model model     (m_path + "/models/jumping_with_animation/Jumping Down.dae");
	Model model     (m_path + "/models/gun/Handgun_dae.dae");
	cout << "      [INSPECT]      " << endl;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		m_path + "/models/jumping_with_animation/Jumping Down.dae",
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
	}
	auto animation = scene->mAnimations[0];
	cout << "Durations : " << animation->mDuration << endl;
	cout << "Ticks per seconds : " << animation->mTicksPerSecond << endl;
	cout << "num Channels : " << animation->mNumChannels << endl;
	
	cout << "All Bones" << endl;
	for (int i = 0; i < animation->mNumChannels; i++)
	{
		cout << animation->mChannels[i]->mNodeName.C_Str() << " | ";
	}
	cout << endl;


	cout << "0th mesh's {" <<  scene->mMeshes[0]->mName.C_Str() 
	<< "} num of bones : " << scene->mMeshes[0]->mNumBones << endl;

	cout << "0th mesh's 0th bone name : " << scene->mMeshes[0]->mBones[0]->mName.C_Str() << endl;

	cout << "Is Tan and BiTan : " << scene->mMeshes[0]->HasTangentsAndBitangents() << endl;

	float bix = scene->mMeshes[1]->mBitangents->x;
	float biy = scene->mMeshes[1]->mBitangents->y;
	float biz = scene->mMeshes[1]->mBitangents->z;
	printf("BiTan : (%f, %f, %f)\n", bix, biy, biz);

	float x = scene->mMeshes[1]->mTangents->x;
	float y = scene->mMeshes[1]->mTangents->y;
	float z = scene->mMeshes[1]->mTangents->z;
	printf("Tan: (%f, %f, %f)\n", x, y, z);
	
	printf("inner of bitan tan : (%f)\n", bix*x + biy*y + biz*z);





	cout << "      [END]      " << endl;




	TM mMatrix, lMatrix, oMatrix;

	float scale_factor { 1.0f };
	glm::mat4 _model_model(1.0f);
	glm::vec3 model_location = glm::vec3(0.0f, 0.1f, 0.0f);
	float angle = glm::radians(-90.0f);
	glm::vec3 angle_vector = glm::vec3(1.0f, 0.0f, 0.0f);

	// mMatrix.model = glm::rotate(mMatrix.model, angle, angle_vector);
	mMatrix.model = glm::scale(mMatrix.model, glm::vec3(scale_factor));
	mMatrix.model = glm::translate(mMatrix.model, model_location);

	oMatrix.model = glm::rotate(oMatrix.model, angle, angle_vector);
	oMatrix.model = glm::scale(oMatrix.model, glm::vec3(scale_factor));
	oMatrix.model = glm::translate(oMatrix.model, model_location);

	// lMatrix.model = glm::scale(lMatrix.model, glm::vec3(scale_factor * 0.3f));


	Screen screen((size_t)WINDOW_WIDTH, (size_t)WINDOW_HEIGHT);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);



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
		
		// view matrix and projection matrix
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection(1.0f);
		projection = glm::perspective(
			glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
		
		mMatrix.projection = projection;
		oMatrix.projection = projection;
		// lMatrix.projection = projection;

		mMatrix.view = view;
		oMatrix.view = view;
		// lMatrix.view = view;

		// light.position = glm::vec3(sin(glfwGetTime()) * 10.0f, 5.0f, cos(glfwGetTime()) * 10.0f);
		light.position = glm::vec3(0.0f, 0.0f, 6.0f);
		light.camera_position = camera.Position;
		lMatrix.model = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.2f)), light.position);

		screen.bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		// 잠깐 Lightball은 없애는걸로
		// glStencilMask(0x00);
		// glFrontFace(GL_CW);
		// lightShader.use();
		// lightShader.setTransformMatrix("matrix", lMatrix);
		// lightShader.setVec3("color", light.color);
		// lightball.Draw(lightShader);

		// glStencilMask(0x00);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		// glFrontFace(GL_CCW);
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


		// glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		// glStencilMask(0x00);
		// glDisable(GL_DEPTH_TEST);
		// // glFrontFace(GL_CCW);
		// outlineShader.use();
		// outlineShader.setTransformMatrix("matrix", oMatrix);
		// outlineShader.setFloat("outlineScale", 0.01f);
		// outline.Draw(outlineShader);
		// glEnable(GL_DEPTH_TEST);
		// glStencilFunc(GL_ALWAYS, 0, 0xFF);
		// glStencilMask(0xFF);

		screen.detach();


		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT);
		screen.Draw(fbShader);
		glEnable(GL_DEPTH_TEST);



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

