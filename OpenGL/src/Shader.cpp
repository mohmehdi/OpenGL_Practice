#include"Shader.h"

#include <fstream>
#include <iostream>
#include<sstream>


#include "Renderer.h"


Shader::Shader(const std::string& filepath)
	:filepath(filepath),m_RendererID(0)
{
	ShaderProgramSource shaderSource = ParseShader(filepath);
	m_RendererID = CreateShader(shaderSource.VertexSource, shaderSource.FragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}



ShaderProgramSource Shader::ParseShader(const std::string filepath)
{

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::stringstream ss[2];

	std::ifstream stream(filepath);
	std::string line;

	ShaderType type = ShaderType::NONE;

	while (std::getline(stream, line))
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
			ss[(int)type] << line << "\n";
		}

	}
	return { ss[0].str(),ss[1].str() };
}
 unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << (type == GL_VERTEX_SHADER ? "vertex " : "fragment") << "shader error : \n" << message;

		glDeleteShader(id);
		return 0;
	}
	return id;
}
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}


void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::UnBind() const
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string& name, float f0, float f1, float f2, float f3)
{
	GLCall(glUniform4f(GetUniformLocation(name), f0, f1, f2, f3));
}
void Shader::SetUniform3f(const std::string& name, float f0, float f1, float f2)
{
	GLCall(glUniform3f(GetUniformLocation(name), f0, f1, f2));
}
void Shader::SetUniform3f(const std::string& name, const glm::vec3& value)
{
	GLCall(glUniform3f(GetUniformLocation(name), value.x,value.y,value.z));
}
void Shader::SetUniform1i(const std::string& name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}
void Shader::SetUniformMat4(const std::string& name,const glm::mat4 &value)
{
	GLCall(glUniformMatrix4fv(GetUniformLocation(name),1,GL_FALSE ,&value[0][0]));
}
int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
	{
		return m_UniformLocationCache[name];
	}

	GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1)
	{
		std::cout << "Uniform : " << name << "Dosent exists\n	";
	}

	m_UniformLocationCache[name] = location;

	return location;
}
