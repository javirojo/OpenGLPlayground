#include <stdio.h>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

int windowHeight = 720;
int windowWidth = 1280;

// Time
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// Camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraSpeed = 12.0f; 
float fov = 45.0f;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastMouseX = 360; // Harcoded set middle point based on initial window size
float lastMouseY = 640; // Harcoded set middle point based on initial window size
float mouseSensitivity = 0.1f;
bool mousePressed = false;

glm::mat4 projection;

//IMGUI state
bool show_demo_window = true;
bool show_another_window = false;

ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

//Shader uniforms
ImVec4 u_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
float uOffsetX = 0.0f;
float uOffsetY = 0.0f;
float uTileX = 1.0f;
float uTileY = 1.0f;


void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	// Update values for calculate projection matrix
	windowHeight = height;
	windowWidth = width;

	glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window)
{
	// Camera movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraPos += cameraSpeed * deltaTime * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraPos -= cameraSpeed * deltaTime * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) 
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	// Restart mouse pressed flag. In that way, when press wi get the actual position of mouse as start point to calculate offset
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_RELEASE)
	{
		mousePressed = false;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		// Start camera movement. Set actual pos in order to keep tracking offset
		if (!mousePressed)
		{
			lastMouseX = xPos;
			lastMouseY = yPos;
			mousePressed = true;
		}

		float xOffset = (xPos - lastMouseX) * mouseSensitivity;
		float yOffset = (lastMouseY - yPos) * mouseSensitivity; // reversed since y-coordinates range from bottom to top
		
		lastMouseX = xPos;
		lastMouseY = yPos;

		yaw += xOffset;
		pitch += yOffset;

		// Keep pitch value between 90 and -90
		if (pitch > 89.0f)
		{
			pitch = 89.0f;
		}
		if (pitch < -89.0f)
		{
			pitch = -89.0f;
		}
	
	
			// Update camera front vector based on new values
			cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			cameraFront.y = sin(glm::radians(pitch));
			cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			cameraFront = glm::normalize(cameraFront);
	}	
}

void ScrollCallback(GLFWwindow* window, double xOfsset, double yOfsset)
{
	fov -= (float)yOfsset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 180.0f)
		fov = 180.0f;
}

void InitIMGUI(GLFWwindow* window)
{
	const char* glsl_version = "#version 130";
	//IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO(); //(void)io;

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void RenderIMGUI()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();


	//ImGUI shader window
	{
		ImGui::Begin("OpenGL Playground");

		ImGui::Text("Shader Parameters");
		ImGui::Separator();

		ImGui::Text("Color");
		ImGui::SameLine();
		ImGui::ColorEdit3("##Color", (float*)&u_color);

		ImGui::Text("uOffsetX");
		ImGui::SameLine();
		ImGui::InputFloat("##uOffsetX", &uOffsetX, 0.1f, 1.0f);

		ImGui::Text("uOffsetY");
		ImGui::SameLine();
		ImGui::InputFloat("##uOffsetY", &uOffsetY, 0.1f, 0.1f);


		ImGui::Text("uTileX");
		ImGui::SameLine();
		ImGui::InputFloat("##uTileX", &uTileX, 1.0f, 1.0f);

		ImGui::Text("uTileY");
		ImGui::SameLine();
		ImGui::InputFloat("##uTileY", &uTileY, 1.0f, 1.0f);
		

		ImGui::Dummy(ImVec2(0.0f, 20.0f));
		ImGui::Text("Camera");
		ImGui::Separator();
		ImGui::Text("Speed");
		ImGui::SameLine();
		ImGui::InputFloat("##camSpeed", &cameraSpeed, 0.1f, 0.1f);
		ImGui::Text("Fov");
		ImGui::SameLine();
		ImGui::InputFloat("##fov", &fov, 1.0f, 1.0f);
		ImGui::Text("Mouse Sensitivity");
		ImGui::SameLine();
		ImGui::InputFloat("##mouseSensitivity", &mouseSensitivity, 0.1f, 0.1f);
		
		ImGui::Dummy(ImVec2(0.0f, 20.0f));
		ImGui::Text("Global Parameters");
		ImGui::Separator();

		ImGui::Text("Delta time %f ms", deltaTime * 1000.0f);
	
		ImGui::Text("Clear Color");
		ImGui::SameLine();
		ImGui::ColorEdit3("##Clear Color", (float*)&clear_color);

		ImGui::End();
	}
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	/*if (show_demo_window)
	{
		ImGui::ShowDemoWindow(&show_demo_window);
	}*/
	
	// Rendering IMGUI always at the end in order to render on Top
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void CleanUpIMGUI()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

int main(void)
{
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
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight,  "OpenGL playground", NULL, NULL);
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

	// Config mouse input mode
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// Callback for window resize. OnResize we update glViewport
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	// Mouse callback
	glfwSetCursorPosCallback(window, MouseCallback);
	//Scroll callback
	glfwSetScrollCallback(window, ScrollCallback);

	// Print which gl version we're using
	std::cout << glGetString(GL_VERSION) << std::endl;

	// Set OpenGL viewport. This will be where OpenGL renders with respect to windows. First point is lower left corner
	glViewport(0, 0, windowWidth, windowHeight);
	
	// Enable Depth testing
	glEnable(GL_DEPTH_TEST);

	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
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

	// Set Vertex Attributes. 
	// Position Attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Texture Attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Unbind Array buffer as information persist in VAO
	// Note that VAO it´s unbind before EBO in order to keep correctly configured
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
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
	unsigned char* textureData = stbi_load("res/textures/woodBoxStylized.png", &width, &height, &nrChannels, 0);
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
		
	// IMGUI Initialization & Config
	InitIMGUI(window);
	
	// Init camera
	cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront.y = sin(glm::radians(pitch));
	cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(cameraFront);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Delta time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Input handling
		ProcessInput(window);

		// Rendering stuff
		glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

		// Wireframe mode 
		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Camera calculations in order to create view matrix
		// We're creating the coordinates system for our camera and the construct loolAt matrix
		// glm do the work for us. We just need to pass camera position, target position, and up vector
		// Here are the calculations to recreate vector by ourselves
		// glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
		// glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
		// glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

		glm::mat4 view;
		
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		// View based on camera transform. As Hazel engine
		/*view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 20.0f));
		view = glm::rotate(view, glm::radians(20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::inverse(view);*/

		// Projection matrix aspect ratio must be calculated based on window resolution
		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
		

		// Set shader. Only MVP matrix need to be updated but keep it all here until abstraction into material
		shader.Bind();
		// Color binding based on ImGUi value
		shader.SetUniform4f("uColor", u_color.x, u_color.y, u_color.z, u_color.w);
		shader.SetUniform1f("uOffsetX", uOffsetX);
		shader.SetUniform1f("uOffsetY", uOffsetY);
		shader.SetUniform1f("uTileX", uTileX);
		shader.SetUniform1f("uTileY", uTileY);
		shader.SetUniform1i("mainTexture", 0);
		
		shader.SetUniformMatrix4("view", view);
		shader.SetUniformMatrix4("projection", projection);
		
		// Draw call
		glBindVertexArray(VAO);
		for (unsigned int i = 0; i < 10; i++)
		{
			float angle = 20.0f * i;		
			glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			shader.SetUniformMatrix4("model", model);
			
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// Always Render ImGUi at last in order to render in front
		RenderIMGUI();
		
		// Double buffer
		glfwSwapBuffers(window);

		// Pooling for input events
		glfwPollEvents();

	}
	CleanUpIMGUI();
	
	glfwTerminate();

	return 0;
}


