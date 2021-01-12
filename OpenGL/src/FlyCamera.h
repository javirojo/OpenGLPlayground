#pragma once
#include "Camera.h"

enum class CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class FlyCamera: public Camera
{
public:
	FlyCamera(float fov, float viewportWidth, float viewportHeight, float nearClip, float FarClip);
	
	// TODO: extract to Camera when transform component defined
	inline glm::vec3 GetPosition() { return m_cameraPosition;  }
	
	void SetViewport(float viewportWidth, float viewportHeight);
	inline float GetFov() const { return m_fov; }
	void SetFov( float fov);
	inline float GetNear() const { return m_nearClip; }
	void SetNear(float near);
	inline float GetFar() const { return m_farClip; }
	void SetFar(float near);
	inline float GetPitch() const { return m_pitch; }
	inline float GetYaw() const { return m_yaw; }

	//TODO: Extract to camera controller??
	inline float GetCameraSpeed() const { return m_cameraSpeed; }
	inline void SetCameraSpeed(float speed) { m_cameraSpeed = speed; };
	void ProcessKeyboard(CameraMovement direction, float deltaTime);
	void ProcessMouse(float xOffset, float yOffset);
	void ProcessScrollbar(float offset);
private:
	void UpdateProjectionMatrix();
	void UpdateViewMatrix();
	void UpdateViewProjectionMatrix();

	float m_fov = 45.0f;
	float m_viewportWidth = 1280.0f; 
	float m_viewportHeight = 720.0f;
	float m_nearClip = 0.1f;
	float m_farClip = 1000.0f;

	glm::vec3 m_cameraPosition = glm::vec3(0.0f, 1.0f, 5.0f);
	glm::vec3 m_cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_cameraUp = glm::vec3(0.0f, 0.0f, 0.0f); 
	glm::vec3 m_cameraRight = glm::vec3(0.0f, 0.0f, 0.0f); 
	glm::vec3 m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f); // TODO: Check if we need to update
	
	float m_yaw = -90.0f;	
	float m_pitch = 0.0f;

	float m_cameraSpeed = 12.0f; //  Extract to camera controller??
};

