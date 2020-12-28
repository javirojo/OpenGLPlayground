#include<stdio.h>
#include <iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

const char* vertexShaderSource =
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource =
"#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"	color = vec4(0.8, 0.2, 0.3, 1.0);\n"
"}\0";

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

unsigned int CreateShaderProgram()
{
	//Compile vertex shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader); // If fails OpenGL not cause any error. We must query how it´s the compilation state
	//vertexShader error checks
	int success;
	char infoLog[512]; //Could be optimized asking for log size
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); // return 0 if any issue with compilation
	if (success == GL_FALSE)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR ON VERTEX SHADER COMPILATION: " << infoLog << std::endl;
	}

	//Compile fragment shader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR ON FRAGMENT SHADER COMPILATION: " << infoLog << std::endl;
	}

	//Create shader program
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram); // If fails OpenGL not cause any error. We must query how it´s the linking state
	//Shader linking error checks
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR ON SHADER LINKING: " << infoLog << std::endl;
	}
	//Once shader program it´s create free shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

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

	float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
	};

	//VBO: 
	// 1.Create 1 buffer and store ID in VBO. This is a reference to an OpenGL Object 
	// 2.Binding VBO to array buffer
	// 3.Copy data to Array buffer which affect to VBO as is the object bound to this buffer
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//VAO: Create and bind
	// IMPORTANT!!! VAO must be bind before setting Vertex attibutes, as "linking" occurs between VAO and attributes not between VBO and VAO
	// Keep in mind that each attribute "links" with data in  VBO 
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Set Vertex Attributes. 
	//In this case we configure position information and set in layout position 0. It must match which shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Unbind Array buffer as information persist in VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Generate shader
	unsigned int shader = CreateShaderProgram();
	//Binding shader to use when draw call 
	glUseProgram(shader);

	//Game loop
	while (!glfwWindowShouldClose(window))
	{
		//Input handling
		ProcessInput(window);

		//Rendering stuff
		glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Draw call
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//Double buffer
		glfwSwapBuffers(window);

		//Pooling for input events
		glfwPollEvents();

	}

	glfwTerminate();
	return 0;
}


