#include <stdio.h>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "Shader.h"


void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

int main(void)
{
	const int windowHeight = 800;
	const int windowWidth = 600;

	glfwInit();

	// Set version of  OpenGL. Default is last supported by GPU
	// Also define Core profile (3.2. and above) vs inmediate mode
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create window and set context
	GLFWwindow* window = glfwCreateWindow(windowHeight, windowWidth, "OpenGL playground", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW windows " << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Initialize GLAD and load all OpenGL function pointers. Always do it before call any OpenGL function!!!
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Print which gl version we're using
	std::cout << glGetString(GL_VERSION) << std::endl;

	// Set OpenGL viewport. This will be where OpenGL renders with respect to windows. First point is lower left corner
	glViewport(0, 0, windowHeight, windowWidth);

	// Callback for window resize. OnResize we update glViewport
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);


	float vertices[] = {
		// positions          // colors           // texture coords
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
	   -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
	   -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};

	unsigned int indices[] = {
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	// VBO: 
	// 1.Create 1 buffer and store ID in VBO. This is a reference to an OpenGL Object 
	// 2.Binding VBO to array buffer
	// 3.Copy data to Array buffer which affect to VBO as is the object bound to this buffer
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// VAO: Create and bind
	// IMPORTANT!!! VAO must be bind before setting Vertex attibutes, as "linking" occurs between VAO and attributes not between VBO and VAO
	// Keep in mind that each attribute "links" with data in  VBO 
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// EBO
	// Important!!! EBO "links" to VAO if VAO is binding and we unbind the EBO, the VAO Wont have and EBO configured and it cause error when drawing indices
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	// Set Vertex Attributes. 
	// In this case we configure position information and set in layout position 0. It must match which shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Unbind Array buffer as information persist in VAO
	// Note that VAO it�s unbind before EBO in order to keep correctly configured
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Texture 	
	// Create texture object
	unsigned int texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Configure texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Loading data 
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char* textureData = stbi_load("res/textures/container.jpg", &width, &height, &nrChannels, 0);
	if (textureData)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Error on loading texture" << std::endl;
	}

	stbi_image_free(textureData);

	// Shader config
	Shader shader = Shader("res/shaders/BasicShader.shader");
	shader.Bind();
	shader.SetUniform4f("uColor", 0.8f, 0.2f, 0.3f, 1.0f);
	shader.SetUniform1i("mainTexture", 0);

	

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Input handling
		ProcessInput(window);

		// Rendering stuff
		glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Wireframe mode 
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Draw call
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Double buffer
		glfwSwapBuffers(window);

		// Pooling for input events
		glfwPollEvents();

	}

	glfwTerminate();
	return 0;
}


