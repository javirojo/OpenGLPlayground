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
	void SetUniform2f(const std::string& name, const glm::vec2 &v);
	void SetUniform2f(const std::string& name, float v1, float v2);
	void SetUniform3f(const std::string& name, const glm::vec3 &v);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform4f(const std::string& name, const glm::vec4 &v);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMatrix2(const std::string& name, const glm::mat2 &mat);
	void SetUniformMatrix3(const std::string& name, const glm::mat3 &mat);
	void SetUniformMatrix4(const std::string& name, const glm::mat4 &mat);
	
private:
	ShaderSource ParseShader(const std::string& filepath);
	unsigned int CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
	unsigned int CompileShader(ShaderType type, const std::string& source);
	int GetUniformLocation(const std::string& name);

	unsigned int m_RendererId;

};



