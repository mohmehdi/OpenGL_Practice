#pragma once

#include "Renderer.h"
class Texture
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	GLuint id;
	int m_width;
	int m_height;
	int m_BBP;
public:

	Texture(std::string& path, const unsigned int slot);

	~Texture();

	void Bind(const unsigned int slot=0)const;
	void UnBind()const;
	inline GLuint GetWidth() const { return this->m_width; }
	inline GLuint GetHeight() const { return this->m_height; }

};