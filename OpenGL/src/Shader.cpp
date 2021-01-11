#include "Shader.h"
#include <glad/glad.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

Shader::Shader(const std::string& filepath)
	: m_RendererId(0)
{
	ShaderSource shaderSource = ParseShader(filepath);
	m_RendererId = CreateShader(shaderSource.VertexShaderSource, shaderSource.FragmentShaderSource);
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererId);
}

ShaderSource Shader::ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	std::string line;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}		
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
{
	unsigned int program = glCreateProgram();
	unsigned int vertexShader = CompileShader(ShaderType::VERTEX, vertexShaderSource);
	unsigned int fragmentShader = CompileShader(ShaderType::FRAGMENT, fragmentShaderSource);
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	int result;
	char infolog[512]; //Could be optimized asking for log size
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if (result == GL_FALSE)
	{
		glGetProgramInfoLog(program, 512, NULL, infolog);
		std::cout << "Error on shader linking: " << infolog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

unsigned int Shader::CompileShader(ShaderType type, const std::string& source)
{
	unsigned int shader = glCreateShader(type == ShaderType::VERTEX ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
	const char* src = source.c_str();
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	int result;
	char infoLog[512]; //Could be optimized asking for log size
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "Error on  " << (type == ShaderType::VERTEX ? "Vertex shader " : "Fragment shader ") << "compilation: " << infoLog << std::endl;
	}

	return shader;
}

void Shader::Bind()
{
	glUseProgram(m_RendererId);
}

void Shader::Unbind()
{
	glUseProgram(0);
}

void Shader::SetUniform1b(const std::string& name, bool value)
{
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1f(const std::string& name, float value)
{
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniformMatrix2(const std::string& name, const glm::mat2& mat)
{
	glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetUniformMatrix3(const std::string& name, const glm::mat3& mat)
{
	glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetUniformMatrix4(const std::string& name, const glm::mat4 &mat)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

int Shader::GetUniformLocation(const std::string& name)
{
	int location = glGetUniformLocation(m_RendererId, name.c_str());
	if (location == -1)
	{
		std::cout << "Warning: uniform " << name << " doesn't exists!" << std::endl;
	}
	else
	{
		return location;
	}
}
