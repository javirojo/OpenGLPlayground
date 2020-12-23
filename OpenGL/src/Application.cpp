#include<stdio.h>
#include <iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>


void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window);

int main(void)
{
	const int height = 800;
	const int width = 600;

	glfwInit();

	//Set version of  OpenGL. Default is last supported by GPU
	//Also define Core profile (3.2. and above) vs inmediate mode
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	//Create window and set context
	GLFWwindow* window = glfwCreateWindow(height, width, "OpenGL playground", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW windows " << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	//Initialize GLAD and load all OpenGL function pointers. Always do it before call any OpenGL function!!!
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Print which gl version we're using
	std::cout << glGetString(GL_VERSION) << std::endl;

	//Set OpenGL viewport. This will be where OpenGL renders with respect to windows. First point is lower left corner
	glViewport(0, 0, height, width);

	//Callback for window resize. OnResize we update glViewport
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

	//Game loop
	while (!glfwWindowShouldClose(window))
	{
		//Input handling
		ProcessInput(window);
		
		//Rendering stuff
		glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Double buffer
		glfwSwapBuffers(window);

		//Pooling for input events
		glfwPollEvents();

	}

	glfwTerminate();
	return 0;
}

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