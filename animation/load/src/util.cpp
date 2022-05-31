#include "util.hpp"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif

using namespace std;

Shader::Shader(string vShader, string fShader)
{
	const char*  vertexPath = vShader.c_str();
	const char*  fragPath = fShader.c_str();
	string strVertexSource, strFragSource;
	ifstream vShaderFile, fShaderFile;
	vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragPath);
		stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close(); fShaderFile.close();
		strVertexSource = vShaderStream.str();
		strFragSource   = fShaderStream.str();
	}
	catch(ifstream::failure e)
	{
		printf("ERROR::SHADER::FILE_CAN'T_READ\n");
	}
	const char* vSource = strVertexSource.c_str();
	const char* fSource = strFragSource.c_str();

	GLuint vertex, fragment;
	int success { 0 };
	char infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vSource, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");
		printf("%s\n", infoLog);
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fSource, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n");
		printf("%s\n", infoLog);
	}

	this->ID = glCreateProgram();
	glAttachShader(this->ID, vertex);
	glAttachShader(this->ID, fragment);
	glLinkProgram(this->ID);

	glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->ID, 512, NULL, infoLog);
		printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
	}
	glDeleteShader(vertex); glDeleteShader(fragment);
}


void Shader::use()
{
	glUseProgram(this->ID);
}

void Shader::setBool(const string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(this->ID, name.c_str()), (int)value);
}

void Shader::setInt(const string& name, int value) const
{
	glUniform1i(glGetUniformLocation(this->ID, name.c_str()), (int)value);
}

void Shader::setFloat(const string& name, float value) const
{
	glUniform1f(glGetUniformLocation(this->ID, name.c_str()), (float)value);
}

void Shader::setMat4(const string& name, glm::mat4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const string& name, glm::vec3 v) const
{
	glUniform3f(glGetUniformLocation(this->ID, name.c_str()), v[0], v[1], v[2]);
}

void Shader::setTransformMatrix(const string& name, CollectionOfTransformMatrix matrix) const
{
	glUniformMatrix4fv(glGetUniformLocation(this->ID, (name + ".model").c_str()), 1, GL_FALSE, glm::value_ptr(matrix.model));
	glUniformMatrix4fv(glGetUniformLocation(this->ID, (name + ".view").c_str()), 1, GL_FALSE, glm::value_ptr(matrix.view));
	glUniformMatrix4fv(glGetUniformLocation(this->ID, (name + ".projection").c_str()), 1, GL_FALSE, glm::value_ptr(matrix.projection));
}

void Shader::setLight(const string&name, Light light) const
{
	if (light.type == lightType::POINT)
	{
		glUniform1f(glGetUniformLocation(this->ID, (name + ".lf_Constant").c_str()), (float)light.att_constant);
		glUniform1f(glGetUniformLocation(this->ID, (name + ".lf_LinearParam").c_str()), (float)light.att_linear);
		glUniform1f(glGetUniformLocation(this->ID, (name + ".lf_QuadParam").c_str()), (float)light.att_quad);
		glUniform1f(glGetUniformLocation(this->ID, (name + ".lf_Power").c_str()), (float)light.power);

		glUniform3f(glGetUniformLocation(this->ID, (name + ".lv_LightColor").c_str()), light.color[0], light.color[1], light.color[2]);
		glUniform3f(glGetUniformLocation(this->ID, (name + ".lv_Position").c_str()), light.position[0], light.position[1], light.position[2]);
		glUniform3f(glGetUniformLocation(this->ID, (name + ".lv_CameraPosition").c_str()), light.camera_position[0], light.camera_position[1], light.camera_position[2]);
	}
	else if (light.type == lightType::SPOT)
	{

	}
	else 
	{

	}
}


GLuint TextureFromFile(const char *path, const string &directory)
{
	// stbi_set_flip_vertically_on_load(true);
	string filename = string(path);
	filename = directory + '/' + filename;
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height, nChannels;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nChannels, 0);
	if (data)
	{
		GLenum format;
		if (nChannels == 1)
			format = GL_RED;
		else if (nChannels == 3)
			format = GL_RGB;
		else if (nChannels == 4)
			format = GL_RGBA;
 
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		printf("Failed to read image file : %s", filename.c_str());
	}
	stbi_image_free(data);
	return textureID;
}

string format_stringi(string &&fmt, int idx)
{
	const char *format = fmt.c_str();
	int size = snprintf(nullptr, 0, format, idx);
	char a[size];
	snprintf(&a[0], size + 1, format, idx);
	string result(a);
	return result;
}


Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) 
		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	this->Position = position;
	this->WorldUp = up;
	this->Yaw = yaw;
	this->Pitch = pitch;
	this->updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	this->Position = glm::vec3(posX, posY, posZ);
	this->WorldUp = glm::vec3(upX, upY, upZ);
	this->Yaw = yaw;
	this->Pitch = pitch;
	this->updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Front, Up);
}

void Camera::updateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
	front.y = sin(glm::radians(this->Pitch));
	front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));

	this->Front = glm::normalize(front);
	this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
	this->Up = glm::normalize(glm::cross(this->Right, this->Front));
}


void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = this->MovementSpeed * deltaTime;
	if (direction == FORWARD)
		this->Position += this->Front * velocity;
	if (direction == BACKWARD)
		this->Position -= this->Front * velocity;
	if (direction == LEFT)
		this->Position -= this->Right * velocity;
	if (direction == RIGHT)
		this->Position += this->Right * velocity;
}
void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= this->MouseSensitivity;
	yoffset *= this->MouseSensitivity;
	this->Yaw += xoffset;
	this->Pitch += yoffset;

	if (constrainPitch)
	{
		if (this->Pitch > 89.0f)
			this->Pitch = 89.0f;
		if (this->Pitch < -89.0f)
			this->Pitch = -89.0f;
	}
	Camera::updateCameraVectors();
}
void Camera::ProcessMouseScroll(float yoffset)
{
	this->Zoom -= (float)yoffset;
	if (this->Zoom < 1.0f)
		this->Zoom = 1.0f;
	if (this->Zoom > 45.0f)
		this->Zoom = 45.0f;
}


Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
		: vertices(vertices), indices(indices), textures(textures)
{
	this->setupMesh();
};

void Mesh::Draw(Shader &shader)
{
	// printf("Indicies: %lu\n", indices.size());
	// printf("Textures size : %lu\n", textures.size());
	for (size_t i = 0; i < textures.size(); i++)
	{
		// printf("NodeName = [%-30s]   size : %lu \t material index : %lu\n", textures[i].node_name.c_str(), textures.size(), textures[i].materialIndex);
		shader.setBool("one_or_more_textures", true);
		glActiveTexture(GL_TEXTURE0 + i);
		string number;
		string uniformName;
		string uniformNameParam;
		if (textures[i].type == "texture_diffuse")
		{
			uniformName = format_stringi("material[%i].ms_Diffuse", textures[i].materialIndex);
			shader.setBool(format_stringi("material[%i].exist_DiffuseMap", textures[i].materialIndex), true);
		}
		else if (textures[i].type == "texture_specular")
		{
			uniformName = format_stringi("material[%i].ms_Specular", textures[i].materialIndex);
			shader.setBool(format_stringi("material[%i].exist_SpecularMap", textures[i].materialIndex), true);
		}
		else if (textures[i].type == "texture_normal")
		{
			uniformName = format_stringi("material[%i].ms_Normal", textures[i].materialIndex);
			shader.setBool(format_stringi("material[%i].exist_NormalMap", textures[i].materialIndex), true);
		} 
		shader.setInt(uniformName.c_str(), (int)i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);

		shader.setFloat(
			format_stringi("material[%i].mf_Shininess", textures[i].materialIndex).c_str(),
			textures[i].shiness);
		shader.setVec3(
			format_stringi("material[%i].mv_AmbientCoeff", textures[i].materialIndex).c_str(),
			textures[i].colorP.ambient);
		shader.setVec3(
			format_stringi("material[%i].mv_DiffuseCoeff", textures[i].materialIndex).c_str(),
			textures[i].colorP.diffuse);
		shader.setVec3(
			format_stringi("material[%i].mv_SpecularCoeff", textures[i].materialIndex).c_str(),
			textures[i].colorP.specular);

	}
	if (textures.size() > 0)
		glActiveTexture(GL_TEXTURE0 + textures[0].materialIndex);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}



void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoord));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Tangent));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Bitangent));
	glBindVertexArray(0);
}


Model::Model(string path)
{
	this->loadModel(path);
}

void Model::Draw(Shader &shader)
{
	for (auto mesh : this->meshes)
	{
		mesh.Draw(shader);
	}
}


void Model::loadModel(string path)
{
	printf(" ========================= START =========================== \n");
	printf("ASSIMP::VERSION::%d.%d\n", aiGetVersionMajor(), aiGetVersionMinor());
	cout << "TARGET: " << path << endl;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		path,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
	}
	this->scene = scene;
	directory = path.substr(0, path.find_last_of('/')); // 나중에 텍스쳐 불러올때 사용
	this->gettingNecessaryData(scene->mRootNode, scene);
	this->processNode(scene->mRootNode, scene);
	printf(" ========================= END =========================== \n");
}


void Model::gettingNecessaryData_from_mesh(aiMesh *mesh, const aiScene *scene)
{
	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 tmpVector3;
		tmpVector3.x = mesh->mVertices[i].x;
		tmpVector3.y = mesh->mVertices[i].y;
		tmpVector3.z = mesh->mVertices[i].z;

		// - - - - - - Model Status - - - - - - // 
		sum_of_vertices.num_vertices++;
		sum_of_vertices.min_x = sum_of_vertices.min_x < tmpVector3.x ? sum_of_vertices.min_x : tmpVector3.x;
		sum_of_vertices.max_x = sum_of_vertices.max_x > tmpVector3.x ? sum_of_vertices.max_x : tmpVector3.x;
		sum_of_vertices.min_y = sum_of_vertices.min_y < tmpVector3.y ? sum_of_vertices.min_y : tmpVector3.y;
		sum_of_vertices.max_y = sum_of_vertices.max_y > tmpVector3.y ? sum_of_vertices.max_y : tmpVector3.y;
		sum_of_vertices.min_z = sum_of_vertices.min_z < tmpVector3.z ? sum_of_vertices.min_z : tmpVector3.z;
		sum_of_vertices.max_z = sum_of_vertices.max_z > tmpVector3.z ? sum_of_vertices.max_z : tmpVector3.z;
	}
}


void Model::gettingNecessaryData(aiNode *node, const aiScene *scene)
{

	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		this->gettingNecessaryData_from_mesh(mesh, scene);
	}

	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		this->gettingNecessaryData(node->mChildren[i], scene);
	}
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
	// cout << "NODE Name : " << node->mName.C_Str() << endl;
	glm::mat4 transformMat = glm::transpose(glm::make_mat4(&node->mTransformation.a1));
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.emplace_back(this->processMesh(mesh, scene, transformMat, node->mName.C_Str()));
	}
	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}
}


Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene, const glm::mat4 transformMat, string nodeName)
{
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;

	double bbox_x = sum_of_vertices.max_x - sum_of_vertices.min_x;
	double bbox_y = sum_of_vertices.max_y - sum_of_vertices.min_y;
	double bbox_z = sum_of_vertices.max_z - sum_of_vertices.min_z;

	double max_size = bbox_x > bbox_y ? bbox_x : bbox_y;
	max_size = max_size > bbox_z ? max_size : bbox_z;

	double x_offset = (sum_of_vertices.max_x + sum_of_vertices.min_x) / 2.0;
	double y_offset = (sum_of_vertices.max_y + sum_of_vertices.min_y) / 2.0;
	double z_offset = (sum_of_vertices.max_z + sum_of_vertices.min_z) / 2.0;

	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 tmpVector3;
		tmpVector3.x = mesh->mVertices[i].x;
		tmpVector3.y = mesh->mVertices[i].y;
		tmpVector3.z = mesh->mVertices[i].z;
		vertex.Position = tmpVector3;

		vertex.Position.x -= x_offset;
		vertex.Position.y -= y_offset;
		vertex.Position.z -= z_offset;

		auto t = transformMat * glm::vec4(vertex.Position, 1.0f);
		vertex.Position.x = t.x;
		vertex.Position.y = t.y;
		vertex.Position.z = t.z;

		vertex.Position.x = vertex.Position.x / (max_size / 2.0);
		vertex.Position.y = vertex.Position.y / (max_size / 2.0);
		vertex.Position.z = vertex.Position.z / (max_size / 2.0);

		tmpVector3.x = mesh->mNormals[i].x;
		tmpVector3.y = mesh->mNormals[i].y;
		tmpVector3.z = mesh->mNormals[i].z;
		vertex.Normal = tmpVector3;

		if (mesh->HasTangentsAndBitangents())
		{
			tmpVector3.x = mesh->mBitangents[i].x;
			tmpVector3.y = mesh->mBitangents[i].y;
			tmpVector3.z = mesh->mBitangents[i].z;
			vertex.Bitangent = tmpVector3;

			tmpVector3.x = mesh->mTangents[i].x;
			tmpVector3.y = mesh->mTangents[i].y;
			tmpVector3.z = mesh->mTangents[i].z;
			vertex.Tangent = tmpVector3;

		}


		t = transformMat * glm::vec4(vertex.Normal, 1.0f);
		vertex.Normal.x = t.x;
		vertex.Normal.y = t.y;
		vertex.Normal.z = t.z;


		if (mesh->mTextureCoords[0])
		{
			glm::vec2 tmpVector2;
			tmpVector2.x = mesh->mTextureCoords[0][i].x;
			tmpVector2.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoord = tmpVector2;
		}
		else
		{
			vertex.TexCoord = glm::vec2(0.0f);
		}


		vertices.emplace_back(vertex);
	}

	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++)
		{
			indices.emplace_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", materialIndex);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", materialIndex);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		vector<Texture> normalMaps = this->loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal", materialIndex);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		materialIndex++;
	}
	for (auto& elem: textures)
	{
		elem.node_name = nodeName;
	}
	return Mesh(vertices, indices, textures);
}
vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, size_t materialIndex)
{
	vector<Texture> textures;
	for (size_t i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		bool skip = false;
		for (const auto elem : textures_loaded)
		{
			if (strcmp(str.C_Str(), elem.path.data()) == 0)
			{
				textures.push_back(elem);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			texture.materialIndex = materialIndex;
			printf("\n<< %s : %s >>\n", typeName.c_str(), texture.path.c_str());
			printf("FILE PATH : %s\n", str.C_Str());

			aiColor4D color;
			aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &color);
			texture.colorP.ambient.x = color.r;
			texture.colorP.ambient.y = color.g;
			texture.colorP.ambient.z = color.b;
			printf("| Ambient coeff (%f, %f, %f) | \n", color.r, color.g, color.b);

			aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color);
			texture.colorP.diffuse.x = color.r;
			texture.colorP.diffuse.y = color.g;
			texture.colorP.diffuse.z = color.b;
			printf("| Diffuse coeff (%f, %f, %f) | \n", color.r, color.g, color.b);

			aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color);
			texture.colorP.specular.x = color.r;
			texture.colorP.specular.y = color.g;
			texture.colorP.specular.z = color.b;
			printf("| Specular coeff (%f, %f, %f) | \n", color.r, color.g, color.b);

			float shine;
			aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &shine);
			printf("| Shiness %f |\n", shine);
			texture.shiness = shine;

			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}


Screen::Screen(size_t width, size_t height) : width(width), height(height)
{
	set();
}

void Screen::set()
{

	float sv[] = {
		// coord    //texture coord
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f };

	glGenVertexArrays(1, &this->vao);
	glGenBuffers(1, &this->vbo);

	glBindVertexArray(this->vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sv), &sv, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(float) * 2));
	glBindVertexArray(0);

	glGenFramebuffers(1, &this->id);
	glBindFramebuffer(GL_FRAMEBUFFER, this->id);

	// color
	glGenTextures(1, &this->color_buffer);
	glBindTexture(GL_TEXTURE_2D, this->color_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->color_buffer, 0);

	// renderbuffer
	glGenRenderbuffers(1, &this->render_buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, this->render_buffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->render_buffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		cout << "[" << __PRETTY_FUNCTION__ << "]" << " " << "FRAMEBUFFER BIND SUCCESSFULLY." << endl;
	}
	else
	{
		throw runtime_error("[FRAMEBUFFER] FRAMEBUFFER HAS FAILED.");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Screen::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->id);
}

void Screen::detach()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Screen::Draw(Shader& shader)
{
	shader.use();
	glBindVertexArray(this->vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->color_buffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}


GLuint LoadCubeMap(vector<string> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nChannels;
	int _n = 0;
	for (const auto elem: faces)
	{
		unsigned char* data = stbi_load(elem.c_str(), &width, &height, &nChannels, 0);
		if (data)
		{
			GLenum format;
			if (nChannels == 1) { format = GL_RED;} 
			else if (nChannels == 3) { format = GL_RGB; }
			else if (nChannels == 4) { format = GL_RGBA; }
			else
			{ 
				const char* fmt = "[%s] IMAGE TYPE NOT SUPPORTED.";
				int sz = snprintf(NULL, 0, fmt, __PRETTY_FUNCTION__);
				char buf[sz + 1];
				snprintf(buf, sizeof(buf), fmt, __PRETTY_FUNCTION__);
				throw runtime_error(buf); 
			}

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _n, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			_n++;
			stbi_image_free(data);
		}
		else
		{
			const char* fmt = "[%s] IMAGE FILE NOT SUPPORTED.";
			int sz = snprintf(NULL, 0, fmt, __PRETTY_FUNCTION__);
			char buf[sz + 1];
			snprintf(buf, sizeof(buf), fmt, __PRETTY_FUNCTION__);
			throw runtime_error(buf); 
			stbi_image_free(data);
		}

	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;

}

CubeMap::CubeMap(string dir_path)
{
	this->set(dir_path);
}

void CubeMap::set(string path)
{
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	
	vector<string> file_vector;
	for (const auto& file : filesystem::directory_iterator(path))
	{
		file_vector.emplace_back(file.path().string());
	}
	sort(file_vector.begin(), file_vector.end());

	cout << " ====== Load cube map ======= " << endl;
	int i = 0;
	for (const auto& elem: file_vector)
	{
		cout << "(" << i << ")" << " [" << __PRETTY_FUNCTION__ << "] " <<
		"ORDER OF CUBE TEXTURE FILES. " <<
		elem.substr(elem.find_last_of('/') + 1) << endl;
		i++;
	}

	// Generate cubemap texture id
	this->texture = LoadCubeMap(file_vector);

	glGenVertexArrays(1, &this->vao);
	glGenBuffers(1, &this->vbo);

	glBindVertexArray(this->vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);
}


void CubeMap::Draw(Shader& shader)
{
	shader.use();
	glBindVertexArray(this->vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}