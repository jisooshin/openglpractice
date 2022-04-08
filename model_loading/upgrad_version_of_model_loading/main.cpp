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
	Shader myShader("shaders/new/ver.glsl", "shaders/new/frag.glsl");
	Model myModel("data/backpack/backpack.obj");


	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		"data/backpack/backpack.obj",
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
	}
	cout << "SUCCESS" << endl;

	long long result = 0;
	float x_min = FLT_MIN;
	float x_max = FLT_MAX;
	float y_min = FLT_MIN;
	float y_max = FLT_MIN;
	float z_min = FLT_MIN;
	float z_max = FLT_MIN;
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		for (int j = 0; j < scene->mMeshes[i]->mNumVertices; j++)
		{
			x_min = x_min > scene->mMeshes[i]->mVertices[j].x ? scene->mMeshes[i]->mVertices[j].x : x_min;
			y_min = y_min > scene->mMeshes[i]->mVertices[j].y ? scene->mMeshes[i]->mVertices[j].y : y_min;
			z_min = z_min > scene->mMeshes[i]->mVertices[j].z ? scene->mMeshes[i]->mVertices[j].z : z_min;

			x_max = x_max < scene->mMeshes[i]->mVertices[j].x ? scene->mMeshes[i]->mVertices[j].x : x_max;
			y_max = y_max < scene->mMeshes[i]->mVertices[j].y ? scene->mMeshes[i]->mVertices[j].y : y_max;
			z_max = z_max < scene->mMeshes[i]->mVertices[j].z ? scene->mMeshes[i]->mVertices[j].z : z_max;
		}
	}
	cout << "vertices.. : "<< result << endl;
	printf("x : (%f, %f)\n", x_min, x_max);
	printf("y : (%f, %f)\n", y_min, y_max);
	printf("z : (%f, %f)\n", z_min, z_max);

	

// 	cout << "diffuse : " << scene->mMaterials[1]->GetTextureCount(aiTextureType_DIFFUSE) << endl;
// 	cout << "spec : "    << scene->mMaterials[1]->GetTextureCount(aiTextureType_SPECULAR) << endl;
// 	cout << "unknown : " << scene->mMaterials[1]->GetTextureCount(aiTextureType_NORMALS) << endl;
// 	cout << endl;
// 
// 	aiString str;	
// 	scene->mMaterials[1]->GetTexture(aiTextureType_DIFFUSE, 0, &str);
// 	cout << str.C_Str() << endl;
// 	str.Clear();
// 	scene->mMaterials[1]->GetTexture(aiTextureType_SPECULAR, 0, &str);
// 	cout << str.C_Str() << endl;
// 	str.Clear();
// 
	
	while(!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.0f, 0.13f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myShader.use();
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection(1.0f);
		projection = glm::perspective(
			glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5));

		myShader.setMat4("view", view);
		myShader.setMat4("projection", projection);
		myShader.setMat4("model", model);

		myModel.Draw(myShader);

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

