#pragma once
#include<string>
#include<unordered_map>
#include<glm.hpp>
struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};


class Shader
{
public:
	Shader(const std::string &filepath);
	~Shader();
	void Bind()const;
	void UnBind()const;

	//set uniforms
	void SetUniform4f(const std::string &name,float f0,float f1,float f2,float f3);
	void SetUniform3f(const std::string& name, float f0, float f1, float f2);
	void SetUniform3f(const std::string& name, const glm::vec3& value);
	void SetUniform1i(const std::string& name, int value);
	void SetUniformMat4(const std::string& name, const glm::mat4& value);
private:
	unsigned int m_RendererID;
	std::string filepath;
	std::unordered_map<std::string, int> m_UniformLocationCache;


	ShaderProgramSource ParseShader(const std::string filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);



	//caching for uniforms
	 int GetUniformLocation(const std::string& name);
};
