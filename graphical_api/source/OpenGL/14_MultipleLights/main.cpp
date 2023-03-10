#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"

float vertices[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};

glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};

const unsigned int SRC_WIDTH = 800;
const unsigned int SRC_HEIGHT = 600;

float lastX = SRC_WIDTH / 2.0f;
float lastY = SRC_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	GLFWwindow * window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "LearnOpenGL", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);	

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	Shader lightCubeShader("F:/github/MO1/MobileOpenGL/bin/shader/MultipleLights_LightCube.vs", "F:/github/MO1/MobileOpenGL/bin/shader/MultipleLights_LightCube.fs");
	Shader materialsShader("F:/github/MO1/MobileOpenGL/bin/shader/MultipleLights.vs","F:/github/MO1/MobileOpenGL/bin/shader/MultipleLights.fs");
	
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);
	
	//??????VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	unsigned int diffuseMap = loadTexture("F:/github/MO1/MobileOpenGL/bin/asset/image/container2.png");
	unsigned int specularMap = loadTexture("F:/github/MO1/MobileOpenGL/bin/asset/image/container2.png");
	materialsShader.setInt("material.diffuse", 0);
	materialsShader.setInt("material.specular",1);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if 1
		glClearColor(0.1, 0.1, 0.1, 1.0);
		//material
		materialsShader.use();
		materialsShader.setVec3("viewPos", camera.Position);
		materialsShader.setFloat("material.shininess", 32);

		//direction
		materialsShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		materialsShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		materialsShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		materialsShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

		//point
		materialsShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		materialsShader.setVec3("pointLights[0].ambient", 0.5f, 0.5f, 0.5f);
		materialsShader.setVec3("pointLights[0].diffuse", 0.8, 0.8, 0.8);
		materialsShader.setVec3("pointLights[0].specular", 1.0, 1.0, 1.0);
		materialsShader.setFloat("pointLights[0].constant", 1.0);
		materialsShader.setFloat("pointLights[0].linear", 0.09);
		materialsShader.setFloat("pointLights[0].quadratic", 0.032);

		materialsShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		materialsShader.setVec3("pointLights[1].ambient", 0.5f, 0.5f, 0.5f);
		materialsShader.setVec3("pointLights[1].diffuse", 0.8, 0.8, 0.8);
		materialsShader.setVec3("pointLights[1].specular", 1.0, 1.0, 1.0);
		materialsShader.setFloat("pointLights[1].constant", 1.0);
		materialsShader.setFloat("pointLights[1].linear", 0.09);
		materialsShader.setFloat("pointLights[1].quadratic", 0.032);

		materialsShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		materialsShader.setVec3("pointLights[2].ambient", 0.5f, 0.5f, 0.5f);
		materialsShader.setVec3("pointLights[2].diffuse", 0.8, 0.8, 0.8);
		materialsShader.setVec3("pointLights[2].specular", 1.0, 1.0, 1.0);
		materialsShader.setFloat("pointLights[2].constant", 1.0);
		materialsShader.setFloat("pointLights[2].linear", 0.09);
		materialsShader.setFloat("pointLights[2].quadratic", 0.032);

		materialsShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		materialsShader.setVec3("pointLights[3].ambient", 0.5f, 0.5f, 0.5f);
		materialsShader.setVec3("pointLights[3].diffuse", 0.8, 0.8, 0.8);
		materialsShader.setVec3("pointLights[3].specular", 1.0, 1.0, 1.0);
		materialsShader.setFloat("pointLights[3].constant", 1.0);
		materialsShader.setFloat("pointLights[3].linear", 0.09);
		materialsShader.setFloat("pointLights[3].quadratic", 0.032);

		//spot
		materialsShader.setVec3("spotLight.position", camera.Position);
		materialsShader.setVec3("spotLight.direction", camera.Front);
		materialsShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		materialsShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		materialsShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		materialsShader.setFloat("spotLight.constant", 1.0f);
		materialsShader.setFloat("spotLight.linear", 0.09);
		materialsShader.setFloat("spotLight.quadratic", 0.032);
		materialsShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		materialsShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
#endif

#ifdef DESERT
		glClearColor(0.75f, 0.52f, 0.3f, 1.0f);

		glm::vec3 pointLightColors[] = {
			glm::vec3(1.0f, 0.6f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 1.0, 0.0),
			glm::vec3(0.2f, 0.2f, 1.0f)
		};

		//material
		materialsShader.use();
		materialsShader.setVec3("viewPos", camera.Position);
		materialsShader.setFloat("material.shininess", 32);

		//direction
		materialsShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		materialsShader.setVec3("dirLight.ambient", 0.3f, 0.24f, 0.14f);
		materialsShader.setVec3("dirLight.diffuse", 0.7f, 0.42f, 0.26f);
		materialsShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

		//point
		materialsShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		materialsShader.setVec3("pointLights[0].ambient", pointLightColors[0] * glm::vec3(0.1));
		materialsShader.setVec3("pointLights[0].diffuse", pointLightColors[0]);
		materialsShader.setVec3("pointLights[0].specular", pointLightColors[0]);
		materialsShader.setFloat("pointLights[0].constant", 1.0);
		materialsShader.setFloat("pointLights[0].linear", 0.09);
		materialsShader.setFloat("pointLights[0].quadratic", 0.032);

		materialsShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		materialsShader.setVec3("pointLights[1].ambient", pointLightColors[1] * glm::vec3(0.1));
		materialsShader.setVec3("pointLights[1].diffuse", pointLightColors[1]);
		materialsShader.setVec3("pointLights[1].specular", pointLightColors[1]);
		materialsShader.setFloat("pointLights[1].constant", 1.0);
		materialsShader.setFloat("pointLights[1].linear", 0.09);
		materialsShader.setFloat("pointLights[1].quadratic", 0.032);

		materialsShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		materialsShader.setVec3("pointLights[2].ambient", pointLightColors[2] * glm::vec3(0.1));
		materialsShader.setVec3("pointLights[2].diffuse", pointLightColors[2]);
		materialsShader.setVec3("pointLights[2].specular", pointLightColors[2]);
		materialsShader.setFloat("pointLights[2].constant", 1.0);
		materialsShader.setFloat("pointLights[2].linear", 0.09);
		materialsShader.setFloat("pointLights[2].quadratic", 0.032);

		materialsShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		materialsShader.setVec3("pointLights[3].ambient", pointLightColors[3] * glm::vec3(0.1));
		materialsShader.setVec3("pointLights[3].diffuse", pointLightColors[3]);
		materialsShader.setVec3("pointLights[3].specular", pointLightColors[3]);
		materialsShader.setFloat("pointLights[3].constant", 1.0);
		materialsShader.setFloat("pointLights[3].linear", 0.09);
		materialsShader.setFloat("pointLights[3].quadratic", 0.032);

		//spot
		materialsShader.setVec3("spotLight.position", camera.Position);
		materialsShader.setVec3("spotLight.direction", camera.Front);
		materialsShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		materialsShader.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.0f);
		materialsShader.setVec3("spotLight.specular", 0.8f, 0.8f, 0.0f);
		materialsShader.setFloat("spotLight.constant", 1.0f);
		materialsShader.setFloat("spotLight.linear", 0.09);
		materialsShader.setFloat("spotLight.quadratic", 0.032);
		materialsShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		materialsShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
#endif

#ifdef FACTORY
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		glm::vec3 pointLightColors[] = {
			glm::vec3(0.2f, 0.2f, 0.6f),
			glm::vec3(0.3f, 0.3f, 0.7f),
			glm::vec3(0.0f, 0.0f, 0.3f),
			glm::vec3(0.4f, 0.4f, 0.4f)
		};

		//material
		materialsShader.use();
		materialsShader.setVec3("viewPos", camera.Position);
		materialsShader.setFloat("material.shininess", 32);

		//direction
		materialsShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		materialsShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.1f);
		materialsShader.setVec3("dirLight.diffuse", 0.2f, 0.2f, 0.7);
		materialsShader.setVec3("dirLight.specular", 0.7f, 0.7f, 0.7f);

		//point
		materialsShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		materialsShader.setVec3("pointLights[0].ambient", pointLightColors[0] * glm::vec3(0.1));
		materialsShader.setVec3("pointLights[0].diffuse", pointLightColors[0]);
		materialsShader.setVec3("pointLights[0].specular", pointLightColors[0]);
		materialsShader.setFloat("pointLights[0].constant", 1.0);
		materialsShader.setFloat("pointLights[0].linear", 0.09);
		materialsShader.setFloat("pointLights[0].quadratic", 0.032);

		materialsShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		materialsShader.setVec3("pointLights[1].ambient", pointLightColors[1] * glm::vec3(0.1));
		materialsShader.setVec3("pointLights[1].diffuse", pointLightColors[1]);
		materialsShader.setVec3("pointLights[1].specular", pointLightColors[1]);
		materialsShader.setFloat("pointLights[1].constant", 1.0);
		materialsShader.setFloat("pointLights[1].linear", 0.09);
		materialsShader.setFloat("pointLights[1].quadratic", 0.032);

		materialsShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		materialsShader.setVec3("pointLights[2].ambient", pointLightColors[2] * glm::vec3(0.1));
		materialsShader.setVec3("pointLights[2].diffuse", pointLightColors[2]);
		materialsShader.setVec3("pointLights[2].specular", pointLightColors[2]);
		materialsShader.setFloat("pointLights[2].constant", 1.0);
		materialsShader.setFloat("pointLights[2].linear", 0.09);
		materialsShader.setFloat("pointLights[2].quadratic", 0.032);

		materialsShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		materialsShader.setVec3("pointLights[3].ambient", pointLightColors[3] * glm::vec3(0.1));
		materialsShader.setVec3("pointLights[3].diffuse", pointLightColors[3]);
		materialsShader.setVec3("pointLights[3].specular", pointLightColors[3]);
		materialsShader.setFloat("pointLights[3].constant", 1.0);
		materialsShader.setFloat("pointLights[3].linear", 0.09);
		materialsShader.setFloat("pointLights[3].quadratic", 0.032);

		//spot
		materialsShader.setVec3("spotLight.position", camera.Position);
		materialsShader.setVec3("spotLight.direction", camera.Front);
		materialsShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		materialsShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		materialsShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		materialsShader.setFloat("spotLight.constant", 1.0f);
		materialsShader.setFloat("spotLight.linear", 0.09);
		materialsShader.setFloat("spotLight.quadratic", 0.032);
		materialsShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
		materialsShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
#endif

#ifdef FACTORY
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		glm::vec3 pointLightColors[] = {
			glm::vec3(0.2f, 0.2f, 0.6f),
			glm::vec3(0.3f, 0.3f, 0.7f),
			glm::vec3(0.0f, 0.0f, 0.3f),
			glm::vec3(0.4f, 0.4f, 0.4f)
		};

		//material
		materialsShader.use();
		materialsShader.setVec3("viewPos", camera.Position);
		materialsShader.setFloat("material.shininess", 32);

		//direction
		materialsShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		materialsShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.1f);
		materialsShader.setVec3("dirLight.diffuse", 0.2f, 0.2f, 0.7);
		materialsShader.setVec3("dirLight.specular", 0.7f, 0.7f, 0.7f);

		//point
		materialsShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		materialsShader.setVec3("pointLights[0].ambient", pointLightColors[0] * glm::vec3(0.1));
		materialsShader.setVec3("pointLights[0].diffuse", pointLightColors[0]);
		materialsShader.setVec3("pointLights[0].specular", pointLightColors[0]);
		materialsShader.setFloat("pointLights[0].constant", 1.0);
		materialsShader.setFloat("pointLights[0].linear", 0.09);
		materialsShader.setFloat("pointLights[0].quadratic", 0.032);

		materialsShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		materialsShader.setVec3("pointLights[1].ambient", pointLightColors[1] * glm::vec3(0.1));
		materialsShader.setVec3("pointLights[1].diffuse", pointLightColors[1]);
		materialsShader.setVec3("pointLights[1].specular", pointLightColors[1]);
		materialsShader.setFloat("pointLights[1].constant", 1.0);
		materialsShader.setFloat("pointLights[1].linear", 0.09);
		materialsShader.setFloat("pointLights[1].quadratic", 0.032);

		materialsShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		materialsShader.setVec3("pointLights[2].ambient", pointLightColors[2] * glm::vec3(0.1));
		materialsShader.setVec3("pointLights[2].diffuse", pointLightColors[2]);
		materialsShader.setVec3("pointLights[2].specular", pointLightColors[2]);
		materialsShader.setFloat("pointLights[2].constant", 1.0);
		materialsShader.setFloat("pointLights[2].linear", 0.09);
		materialsShader.setFloat("pointLights[2].quadratic", 0.032);

		materialsShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		materialsShader.setVec3("pointLights[3].ambient", pointLightColors[3] * glm::vec3(0.1));
		materialsShader.setVec3("pointLights[3].diffuse", pointLightColors[3]);
		materialsShader.setVec3("pointLights[3].specular", pointLightColors[3]);
		materialsShader.setFloat("pointLights[3].constant", 1.0);
		materialsShader.setFloat("pointLights[3].linear", 0.09);
		materialsShader.setFloat("pointLights[3].quadratic", 0.032);

		//spot
		materialsShader.setVec3("spotLight.position", camera.Position);
		materialsShader.setVec3("spotLight.direction", camera.Front);
		materialsShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		materialsShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		materialsShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		materialsShader.setFloat("spotLight.constant", 1.0f);
		materialsShader.setFloat("spotLight.linear", 0.09);
		materialsShader.setFloat("spotLight.quadratic", 0.032);
		materialsShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
		materialsShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
#endif

		//mvp
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SRC_WIDTH / (float)SRC_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		materialsShader.setMat4("projection",projection);
		materialsShader.setMat4("view",view);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE0, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		//render cube
		glBindVertexArray(cubeVAO);
		for (int i = 0; i < 10; ++i)
		{
			glm::mat4 model;
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			materialsShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//draw lamp object
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		glBindVertexArray(lightCubeVAO);
		for (int i = 0; i < 4; ++i)
		{
			glm::mat4 model = glm::mat4(1.0);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2));
			lightCubeShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightCubeVAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
