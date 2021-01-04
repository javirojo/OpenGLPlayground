#pragma once
#include <string>
#include <glm/glm.hpp>

enum class ShaderType
{
	NONE = -1,
	VERTEX = 0,
	FRAGMENT = 1
};

struct ShaderSource
{
	std::string VertexShaderSource;
	std::string FragmentShaderSource;
};


class Shader
{
	
public:
	Shader(const std::string& filepath);
	~Shader();
	void Bind();
	void Unbind();
	void SetUniform1b(const std::string& name, bool value);
	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMatrix2(const std::string& name, glm::mat2 &mat);
	void SetUniformMatrix3(const std::string& name, glm::mat3 &mat);
	void SetUniformMatrix4(const std::string& name, glm::mat4 &mat);
	

private:
	unsigned int m_RendererId;

	ShaderSource ParseShader(const std::string& filepath);
	unsigned int CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
	unsigned int CompileShader(ShaderType type, const std::string& source);
	
	int GetUniformLocation(const std::string& name);
};



