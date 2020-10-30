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

// lighting
glm::vec3 lightPos(0, 2, -1);

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

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

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

    // build and compile our shader zprogram
    Shader ourShader("src/shader.vs", "src/shader.fs");
	Shader lightCubeShader("src/shaderLightSource.vs", "src/shaderLightSource.fs");

	// configure cube vao and vbo
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//configure light
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	// VBO already bound

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

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

        // activate shader
        ourShader.use();
		ourShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		ourShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		ourShader.setVec3("lightPos", lightPos);
		ourShader.setVec3("viewPos", glm::vec3(2.0f, 4.0f, -5.0f));

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
		glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 4.0f, -5.0f), glm::vec3(0, 2, -1), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("view", view);
		

        // render boxes
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 17; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            ourShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

		// light source
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.5f));
		lightCubeShader.setMat4("model", model);

		glBindVertexArray(lightCubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

        // glfw: swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // de-allocate all resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate
    glfwTerminate();
    return 0;
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