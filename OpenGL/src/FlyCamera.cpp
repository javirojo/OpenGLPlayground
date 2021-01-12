#include "FlyCamera.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

FlyCamera::FlyCamera(float fov, float viewportWidth, float viewportHeight, float nearClip, float FarClip)
	:m_fov(fov), m_viewportWidth(viewportWidth), m_viewportHeight(viewportHeight), m_nearClip(nearClip), m_farClip(FarClip)
{
	//Hardcoded default transform
	m_yaw = -120.0f;
	m_pitch = -30.0f;
	m_cameraPosition = glm::vec3(2.0f, 4.0f, 8.0f);

	m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_viewportWidth / m_viewportHeight, m_nearClip, m_farClip);
	UpdateViewMatrix();
}

void FlyCamera::SetViewport(float viewportWidth, float viewportHeight)
{
	m_viewportWidth = viewportWidth;
	m_viewportHeight = viewportHeight;
	UpdateProjectionMatrix();
}

void FlyCamera::SetNear(float near) {
	m_nearClip = near; 
	UpdateProjectionMatrix();
}

void FlyCamera::SetFar(float far) {
	m_farClip = far;
	UpdateProjectionMatrix();
}

void FlyCamera::UpdateProjectionMatrix()
{
	m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_viewportWidth / m_viewportHeight, m_nearClip, m_farClip);
	UpdateViewProjectionMatrix();
}

void FlyCamera::UpdateViewMatrix() 
{	
	m_cameraDirection.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_cameraDirection.y = sin(glm::radians(m_pitch));
	m_cameraDirection.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_cameraDirection = glm::normalize(m_cameraDirection);

	// Recalculate camera vectors based on new position in order to generate lookAt matrix
	m_cameraRight = glm::normalize(glm::cross(m_cameraDirection, m_worldUp));
	m_cameraUp = glm::normalize(glm::cross(m_cameraRight, m_cameraDirection));

	m_viewMatrix = glm::lookAt(m_cameraPosition, m_cameraPosition + m_cameraDirection, m_cameraUp);

	UpdateViewProjectionMatrix();
}

void FlyCamera::UpdateViewProjectionMatrix()
{
	m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
}


//TODO: limit Fov values ???
void FlyCamera::SetFov(float fov)
{
	m_fov = fov;
	UpdateProjectionMatrix();
}

void  FlyCamera::ProcessKeyboard(CameraMovement direction, float deltaTime)
{
	if (direction == CameraMovement::FORWARD)
	{
		m_cameraPosition += m_cameraSpeed * deltaTime * m_cameraDirection;
		UpdateViewMatrix();
	}
	if (direction == CameraMovement::BACKWARD)
	{
		m_cameraPosition -= m_cameraSpeed * deltaTime * m_cameraDirection;
		UpdateViewMatrix();
	}
	if (direction == CameraMovement::LEFT)
	{
		m_cameraPosition -= m_cameraRight * m_cameraSpeed * deltaTime;
		UpdateViewMatrix();
	}
	if (direction == CameraMovement::RIGHT)
	{
		m_cameraPosition += m_cameraRight * m_cameraSpeed * deltaTime;
		UpdateViewMatrix();
	}
}

void FlyCamera::ProcessMouse(float xOffset, float yOffset)
{
		m_yaw += xOffset;
		m_pitch += yOffset;

		// Keep pitch value between 90 and -90
		if (m_pitch > 89.0f)
		{
			m_pitch = 89.0f;
		}
		if (m_pitch < -89.0f)
		{
			m_pitch = -89.0f;
		}

		UpdateViewMatrix();
}

void FlyCamera::ProcessScrollbar(float offset)
{
	m_fov -= offset;
	SetFov(m_fov);
}