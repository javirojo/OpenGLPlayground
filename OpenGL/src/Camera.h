#pragma once

#include "glm/glm.hpp"

// TODO: Add cameraType ??
class Camera
{
	
public:	
	Camera() = default;
	virtual ~Camera() = default;
	inline const glm::vec4 GetClearColor() const { return  m_clearColor; }
	inline void SetClearColor(glm::vec4 color) { m_clearColor = color; }
	inline const glm::mat4 GetProjectionMatrix() const { return m_projectionMatrix; }
	inline const glm::mat4 GetViewMatrix() const { return m_viewMatrix; }
	inline const glm::mat4 GetProjectionViewMatrix() const { return m_viewProjectionMatrix; }

protected:
	glm::vec4 m_clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f );
	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;
	glm::mat4 m_viewProjectionMatrix;
};

