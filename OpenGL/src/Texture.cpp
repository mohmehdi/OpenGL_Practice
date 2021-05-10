#include"Texture.h"
#include<iostream>
#include<string>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include"stb_image.h"

Texture::Texture(std::string& path,const unsigned int slot)
{
	stbi_set_flip_vertically_on_load(true);
	m_LocalBuffer = stbi_load(path.c_str(), &m_width, &m_height, &m_BBP, 4);

	GLCall(glGenTextures(1, &m_RendererID));

	Bind(slot);

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE,m_LocalBuffer));
	//GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	if(m_LocalBuffer)
	stbi_image_free(m_LocalBuffer);
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1,&m_RendererID));
}

void Texture::Bind(const unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	GLCall(glBindTexture(GL_TEXTURE_2D,m_RendererID));
}

void Texture::UnBind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
