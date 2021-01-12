#include <stdio.h>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Shader.h"
#include "FlyCamera.h"

int windowHeight = 720;
int windowWidth = 1280;

// Time
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// Input
float lastMouseX = 0; 
float lastMouseY = 0; 
float mouseSensitivity = 0.1f;
bool mousePressed = false;

// Camera 
FlyCamera camera (45.0f, windowWidth, windowHeight, 0.1f, 1000.0f);

// Camera editor
// TODO: Think how to abstract IMgui parameters from classes
float cameraNearEditor = camera.GetNear();
float cameraFarEditor = camera.GetFar();
float cameraFovEditor = camera.GetFov();
float cameraSpeedEditor = camera.GetCameraSpeed(); 
glm::vec4 initColor = camera.GetClearColor();
ImVec4 clearColorEditor = ImVec4(initColor.x, initColor.y, initColor.z, initColor.w);

//IMGUI state
bool show_demo_window = true;
bool show_another_window = false;

// lighting
float ambientLightIntensity = 0.1f;
glm::vec4 ambientLightColor(0.3f, 0.82f, 0.74f, 1.0f);

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec4 lightColor(1.0f, 1.0f, 1.0f, 1.0f);
float lightIntensity = 1.0f;

glm::vec4 specularColor(0.0f, 1.0f, 0.0f, 1.0f);
float specularShininess = 64.0f;

glm::vec3 cubePos(0.0f, 0.6f, 0.0f);


//Shader uniforms
ImVec4 u_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
float uOffsetX = 0.0f;
float uOffsetY = 0.0f;
float uTileX = 1.0f;
float uTileY = 1.0f;
bool blinn = true;


void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	// Update windows size values 
	windowHeight = height;
	windowWidth = width;

	camera.SetViewport(width, height);
	glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window)
{
	// Camera movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(CameraMovement::FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(CameraMovement::BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(CameraMovement::LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) 
	{
		camera.ProcessKeyboard(CameraMovement::RIGHT, deltaTime);
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

		camera.ProcessMouse(xOffset, yOffset);
	}	
}

void ScrollCallback(GLFWwindow* window, double xOfsset, double yOfsset)
{
	camera.ProcessScrollbar((float)yOfsset);
	cameraFovEditor = camera.GetFov();	
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

		ImGui::Text("Cube Parameters");
		ImGui::Separator();
		ImGui::Text("Position");
		ImGui::SameLine();
		ImGui::DragFloat3("##cubePosition", (float*)&cubePos);
		ImGui::Text("Color");
		ImGui::SameLine();
		ImGui::ColorEdit3("##Color", (float*)&u_color);

		ImGui::Text("Specular color");
		ImGui::SameLine();
		ImGui::ColorEdit3("##specularColor", (float*)&specularColor);
		ImGui::Text("Specular shininess");
		ImGui::SameLine();
		ImGui::DragFloat("##specularShininess", (float*)&specularShininess, 1.0f, 2.0f, 1024.0f);

		ImGui::Dummy(ImVec2(0.0f, 10.0f));

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
		ImGui::Text("Lightning");
		ImGui::Separator();

		ImGui::Text("Blinn");
		ImGui::SameLine();
		ImGui::Checkbox("##blinn", &blinn);

		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		ImGui::Text("AMBIENT LIGHT");
		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		ImGui::Text("Ambient light Color");
		ImGui::SameLine();
		ImGui::ColorEdit3("##ambientLightColor", (float*)&ambientLightColor);
		ImGui::Text("Ambient light intensity");
		ImGui::SameLine();
		ImGui::DragFloat("##ambientLightIntensity", (float*)&ambientLightIntensity, 0.1f, 0.1f, 10.0f);
		
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Text("POINT LIGHT");
		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		ImGui::Text("Color");
		ImGui::SameLine();
		ImGui::ColorEdit3("##LightColor", (float*)&lightColor);
		ImGui::Text("Position");
		ImGui::SameLine();
		ImGui::DragFloat3("##LightPosition", (float*)&lightPos);
		ImGui::Text("Light intensity");
		ImGui::SameLine();
		ImGui::DragFloat("##lightIntensity", (float*)&lightIntensity, 0.1f, 0.1f, 10.0f);
		
		
		ImGui::Dummy(ImVec2(0.0f, 20.0f));
		ImGui::Text("Camera");
		ImGui::Separator();
		ImGui::Text("Clear Color");
		ImGui::SameLine();
		if (ImGui::ColorEdit3("##Clear Color", (float*)&clearColorEditor))
		{
			camera.SetClearColor(glm::vec4(clearColorEditor.x, clearColorEditor.y, clearColorEditor.z, clearColorEditor.w));
		}
		
		ImGui::Text("Near clip plane");
		ImGui::SameLine();
		if (ImGui::InputFloat("##nearClipPlane", &cameraNearEditor, 1.0f, 1.0f))
		{
			camera.SetNear(cameraNearEditor);
		}

		ImGui::Text("Far clip plane");
		ImGui::SameLine();
		if (ImGui::InputFloat("##farClipPlane", &cameraFarEditor, 1.0f, 1.0f))
		{
			camera.SetFar(cameraFarEditor);
		}

		ImGui::Text("Fov");
		ImGui::SameLine();
		if (ImGui::InputFloat("##fov", &cameraFovEditor, 1.0f, 1.0f))
		{
			camera.SetFov(cameraFovEditor);
		}

		ImGui::Text("Speed");
		ImGui::SameLine();
		if (ImGui::InputFloat("##camSpeed", &cameraSpeedEditor, 0.1f, 0.1f))
		{
			camera.SetCameraSpeed(cameraSpeedEditor);
		}


		ImGui::Text("Mouse Sensitivity");
		ImGui::SameLine();
		ImGui::InputFloat("##mouseSensitivity", &mouseSensitivity, 0.1f, 0.1f);
		
		ImGui::Dummy(ImVec2(0.0f, 20.0f));
		ImGui::Text("Global Parameters");
		ImGui::Separator();

		ImGui::Text("Delta time %f ms", deltaTime * 1000.0f);
	
		

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

	// Enable Depth testing
	glEnable(GL_DEPTH_TEST);

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	// VBO: 
	// 1.Create 1 buffer and store ID in VBO. This is a reference to an OpenGL Object 
	// 2.Binding VBO to array buffer
	// 3.Copy data to Array buffer which affect to VBO as is the object bound to this buffer
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Cube VAO:
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
 
	// Position Attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Normal Attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Texture Attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// Cube VAO:
	unsigned int LightVAO;
	glGenVertexArrays(1, &LightVAO);
	glBindVertexArray(LightVAO);

	// Position Attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	// Diffuse Texture 	
	unsigned int diffuseTexture;
	glGenTextures(1, &diffuseTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseTexture);

	// Configure texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Loading data 
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char* textureData = stbi_load("res/textures/container2.png", &width, &height, &nrChannels, 0);
	if (textureData)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Error on loading texture" << std::endl;
	}

	// Create texture object
	unsigned int specularTexture;
	glGenTextures(1, &specularTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularTexture);

	// Configure texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Loading data 
	int widthSpec, heightSpec, nrChannelsSpec;
	unsigned char* textureSpecData = stbi_load("res/textures/container2_specular.png", &widthSpec, &heightSpec, &nrChannelsSpec, 0);
	if (textureSpecData)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthSpec, heightSpec, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureSpecData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Error on loading specular texture" << std::endl;
	}

	// Loading data 
	unsigned int planeTexture;
	glGenTextures(1, &planeTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, planeTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int widthPlane, heightPlane, nrChannelsPlane;
	unsigned char* texturePlaneData = stbi_load("res/textures/wood.png", &widthPlane, &heightPlane, &nrChannelsPlane, 0);
	if (texturePlaneData)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthPlane, heightPlane, 0, GL_RGB, GL_UNSIGNED_BYTE, texturePlaneData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Error on loading plane texture" << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// Loading data 
	unsigned int whiteTexture;
	glGenTextures(1, &whiteTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, whiteTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int widthWhite, heightWidth, nrChannelsWhite;
	unsigned char* textureWhiteData = stbi_load("res/textures/wood.png", &widthWhite, &heightWidth, &nrChannelsWhite, 0);
	if (textureWhiteData)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthWhite, heightWidth, 0, GL_RGB, GL_UNSIGNED_BYTE, textureWhiteData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Error on loading white texture" << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(textureData);
	stbi_image_free(textureWhiteData);
	stbi_image_free(texturePlaneData);
	stbi_image_free(textureSpecData);

	// Shader config
	Shader shader = Shader("res/shaders/BasicShader.shader");
	Shader lightCasterShader = Shader("res/shaders/PlainColorShader.shader");
		
	// IMGUI Initialization & Config
	InitIMGUI(window);

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
		glm::vec4 clearColor = camera.GetClearColor();
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glBindVertexArray(VAO);

		//Cube rendering
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularTexture);
		shader.Bind();
		shader.SetUniform1i("blinn", blinn);
		shader.SetUniform4f("uColor", u_color.x, u_color.y, u_color.z, u_color.w);
		shader.SetUniform1f("uOffsetX", uOffsetX);
		shader.SetUniform1f("uOffsetY", uOffsetY);
		shader.SetUniform1f("uTileX", uTileX);
		shader.SetUniform1f("uTileY", uTileY);
		shader.SetUniform1i("diffuseTexture", 0);
		shader.SetUniform1i("specularTexture", 1);

		shader.SetUniform4f("uAmbientColor", ambientLightColor.x, ambientLightColor.y, ambientLightColor.z, ambientLightColor.w);
		shader.SetUniform1f("uAmbientIntensity", ambientLightIntensity);
		// TODO: Light color
		shader.SetUniform3f("uLightPosition", lightPos.x, lightPos.y, lightPos.z);
		shader.SetUniform4f("uLightColor", lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		shader.SetUniform1f("uLightIntensity", lightIntensity);
		shader.SetUniform4f("uSpecularColor", specularColor.x, specularColor.y, specularColor.z, specularColor.w);	
		shader.SetUniform1f("uSpecularShininess", specularShininess);
		shader.SetUniform3f("uViewPosition",camera.GetPosition());
		
		shader.SetUniformMatrix4("view", camera.GetViewMatrix());
		shader.SetUniformMatrix4("projection", camera.GetProjectionMatrix());
		
		glm::mat4 model = glm::translate(glm::mat4(1.0f), cubePos);
		shader.SetUniformMatrix4("model", model);
		// Normal matrix calculated in world space and passed as uniform to avoid recaculate in each pixel shader
		glm::mat4 normalMatrix = glm::transpose(glm::inverse(model));
		shader.SetUniformMatrix4("normalMatrix", normalMatrix);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		//Plane
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, planeTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, whiteTexture);
		shader.SetUniform1f("uTileX", 5.0f);
		shader.SetUniform1f("uTileY", 5.0f);
		glm::mat4 planeModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		planeModel = glm::scale(planeModel, glm::vec3(10.5f, 0.1f, 10.0f));
		shader.SetUniformMatrix4("model", planeModel);
		glm::mat4 planeNormalMatrix = glm::transpose(glm::inverse(model));
		shader.SetUniformMatrix4("normalMatrix", normalMatrix);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Ligth rendering
		glBindVertexArray(LightVAO);

		lightCasterShader.Bind();
		lightCasterShader.SetUniform4f("uLightColor", lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		lightCasterShader.SetUniform1f("uLightIntensity", lightIntensity);
		lightCasterShader.SetUniformMatrix4("view", camera.GetViewMatrix());
		lightCasterShader.SetUniformMatrix4("projection", camera.GetProjectionMatrix());
		
		glm::mat4 ligthModel = glm::translate(glm::mat4(1.0f), lightPos);
		ligthModel = glm::scale(ligthModel, glm::vec3(0.2f));
		lightCasterShader.SetUniformMatrix4("model", ligthModel);
		
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
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


