#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Shader.h"

#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void renderCube();
void renderScene(const Shader& shader, const glm::vec3 cubePos[]);

// calculation functions
int calcCorrectIndex(int index);
std::vector<glm::vec3> calcTangents(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
glm::vec3 calcPoint(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 tang1, glm::vec3 tang2);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

std::vector<glm::vec3> tangents;
//num of points for camerapath
const int CAMERPATHLENGTH = 20;
//increment for T
float increment = 0.005f;

unsigned int planeVAO;

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

	// build and compile our shader zprogram
	Shader ourShader("src/shader.vs", "src/shader.fs");
	//Shader lightCubeShader("src/shaderLightSource.vs", "src/shaderLightSource.fs");
	Shader depthShader("src/depthShader.vs", "src/depthShader.fs");

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // world space positions cubes
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(0.0f,  2.0f, 3.0f),
		glm::vec3(0.0f,  0.0f, 6.0f),
		glm::vec3(0.0f,  1.0f, 8.0f),
		glm::vec3(0.0f,  0.0f, 10.0f),
		glm::vec3(0.0f,  -3.0f, 12.0f),
		glm::vec3(5.0f,  0.0f, 3.0f),
		glm::vec3(5.0f,  0.0f, 6.0f),
		glm::vec3(5.0f,  5.0f, 8.0f),
		glm::vec3(5.0f,  0.0f, 10.0f),
		glm::vec3(7.0f,  0.0f, 12.0f),
		glm::vec3(-5.0f,  0.0f, 3.0f),
		glm::vec3(-8.0f,  0.0f, 6.0f),
		glm::vec3(-6.0f,  3.0f, 8.0f),
		glm::vec3(-5.0f,  0.0f, 10.0f),
		glm::vec3(-6.0f,  0.0f, 12.0f),
		glm::vec3(-1, 2, -1.0f)
    };

    // world space position of path
	glm::vec3 pathPos[] = {
		glm::vec3(0.0f,  5.0f,  -3.0f),
		glm::vec3(1.0f,  3.0f,  -1.0f),
		glm::vec3(2.0f,  3.0f,  1.0f),
		glm::vec3(2.0f,  3.0f,  0.0f),
		glm::vec3(4.0f,  3.0f,  4.0f),
		glm::vec3(3.0f,  2.0f,  8.0f),
		glm::vec3(2.0f,  1.0f,  10.0f),
		glm::vec3(1.0f,  1.0f,  12.0f),
		glm::vec3(4.0f,  0.0f,  14.0f),
		glm::vec3(2.0f,  2.0f,  20.0f),
		glm::vec3(0.0f,  3.0f,  14.0f),
		glm::vec3(-2.0f,  5.0f,  12.0f),
		glm::vec3(-2.0f,  4.0f,  10.0f),
		glm::vec3(-2.0f,  3.0f,  8.0f),
		glm::vec3(-2.0f,  2.0f,  6.0f),
		glm::vec3(-2.0f,  0.0f,  4.0f),
		glm::vec3(-2.0f,  0.0f,  2.0f),
		glm::vec3(-2.0f,  0.0f,  0.0f),
		glm::vec3(-2.0f,  0.0f,  -2.0f),
		glm::vec3(-1.0f,  0.0f,  -2.0f)
	};

	glm::vec3 lookDir[]{
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(1.0f, 0.0f, 1.0f),
        glm::vec3(1.0f, 0.0f, 1.0f),
        glm::vec3(0.3f, 0.0f, 1.0f),
        glm::vec3(0.1f, 0.0f, 1.0f),
        glm::vec3(0.1, 0, 0.7),
        glm::vec3(0.3, 0, 0.5),
        glm::vec3(-1, 0, -1),
        glm::vec3(-0.4, 0, -0),
        glm::vec3(-0.7, 0, -1),
        glm::vec3(-1, 0, -1),
        glm::vec3(-1, 0, -0.8),
        glm::vec3(-1, 0, -0.5),
        glm::vec3(-1, 0, -0.3),
        glm::vec3(-1, 0, 0),
        glm::vec3(-1, 0, 0.1),
        glm::vec3(-1, 0, 0.5),
        glm::vec3(-1, 0, 1),
        glm::vec3(-1, 0, 1),
        glm::vec3(0, 0, 1),
	};

	float planeVertices[] = {
		//// positions            // normals
		// 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,
		//-10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,
		//-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,

		// 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,
		//-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,
		// 10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f

		 // positions            // normals       
		 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,
		-25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,

		 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f, 
		 25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f
	};

	// plane VAO
	unsigned int planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	// configure depth map FBO
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ourShader.use();
	ourShader.setInt("shadowMap", 0);

	// lighting info
	glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

	// vars for calculation
	float t = 0.0f;
	int currentPointIndex = 1;
	glm::quat lookDirQuaternions[20];
	glm::vec3 initialOrientation = glm::vec3(0.0f, 0.0f, -1.0f);

    //transform lookDir vectors to Quaternions
	for (int i = 0; i < 20; i++) {
		lookDirQuaternions[i] = glm::rotation(glm::normalize(initialOrientation), glm::normalize(lookDir[i]));
	}

    // render loop
    while (!glfwWindowShouldClose(window))
    {
		if (t < 1) {
            t += increment;
		}
		else {
			currentPointIndex = calcCorrectIndex(currentPointIndex);
			t = 0.0f;
		}

		if (currentPointIndex > CAMERPATHLENGTH) {
			currentPointIndex = 1;
		}  

        // input
        processInput(window);

		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. render depth of scene to texture (from light's perspective)
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = -10.0f, far_plane = 20.0f;
		lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		depthShader.use();
		depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		renderScene(depthShader, cubePositions);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		

        // activate shader
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ourShader.use();
		// pass projection matrix to shader
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);

		tangents = calcTangents(pathPos[currentPointIndex - 1], pathPos[currentPointIndex], pathPos[currentPointIndex + 1], pathPos[currentPointIndex + 2]);
		//calculate the helper quats for p0 and p1
		glm::quat helpQuat1 = glm::intermediate(lookDirQuaternions[currentPointIndex - 1], lookDirQuaternions[currentPointIndex], lookDirQuaternions[currentPointIndex + 1]);
		glm::quat helpQuat2 = glm::intermediate(lookDirQuaternions[currentPointIndex], lookDirQuaternions[currentPointIndex + 1], lookDirQuaternions[currentPointIndex + 2]);
		// calculate the point the camera will move to and the direction it will look
		glm::vec3 movePoint = calcPoint(t, pathPos[currentPointIndex], pathPos[currentPointIndex + 1], tangents[0], tangents[1]);
		glm::quat lookQuat = glm::squad(lookDirQuaternions[currentPointIndex], lookDirQuaternions[currentPointIndex + 1], helpQuat1, helpQuat2, t);

		// camera/view transformation
		//glm::mat4 view = glm::lookAt(movePoint, movePoint + lookQuat * initialOrientation, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 4.0f, -5.0f), glm::vec3(0, -1, 2), glm::vec3(0.0f, 1.0f, 0.0f));
		
		ourShader.setMat4("view", view);

		ourShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		ourShader.setVec3("viewPos", glm::vec3(0, 0, 0));
		//ourShader.setVec3("viewPos", movePoint);
		ourShader.setVec3("lightPos", lightPos);
		ourShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		renderScene(ourShader, cubePositions);

		// light source
		/*lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.5f));
		lightCubeShader.setMat4("model", model);*/

		/*glBindVertexArray(lightCubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);*/

        // glfw: swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // de-allocate all resources
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);

    // glfw: terminate
    glfwTerminate();
    return 0;
}

void renderScene(const Shader& shader, const glm::vec3 cubePos[])
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
	shader.setMat4("model", model);
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	for (unsigned int i = 0; i < 17; i++)
	{
		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePos[i]);
		shader.setMat4("model", model);
		renderCube();
	}
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
	if (cubeVAO == 0)
	{
		// vertex data
		float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
		};
		// configure cube vao and vbo
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);

		glBindVertexArray(cubeVAO);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

// process all input
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);      
}

//  window size
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// calculate index for points, modulo for array reset
int calcCorrectIndex(int index) {
	index++;
	int mod = index % 18;
	if (mod == 0) {
		return 1;
	}
	return mod;
}

//calculate tangents p1 and p2 as helper values
std::vector<glm::vec3> calcTangents(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	float t = 0.0f;
	float b = 0.0f;
	float c = 0.0f;

	std::vector<glm::vec3> results(2);

	float coef1 = ((1 - t) * (1 + b) * (1 + c)) / 2;
	float coef2 = ((1 - t) * (1 - b) * (1 - c)) / 2;
	float coef3 = ((1 - t) * (1 + b) * (1 - c)) / 2;
	float coef4 = ((1 - t) * (1 - b) * (1 + c)) / 2;

	glm::vec3 tang1 = coef1 * (p1 - p0) + coef2 * (p2 - p1);
	glm::vec3 tang2 = coef3 * (p2 - p1) + coef4 * (p3 - p2);
	results[0] = tang1;
	results[1] = tang2;
	return results;
}

//calculate the point between p0 and p1 based on the t value
glm::vec3 calcPoint(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 tang1, glm::vec3 tang2) {
	float t2 = t * t;
	float t3 = t * t * t;

	float h00 = 2 * t2 - 3 * t2 + 1;
	float h10 = t3 - 2 * t2 + t;
	float h01 = -2 * t3 + 3 * t3;
	float h11 = t3 - t2;

	return h00 * p0 + h10 * tang1 + h01 * p1 + h11 * tang2;
}