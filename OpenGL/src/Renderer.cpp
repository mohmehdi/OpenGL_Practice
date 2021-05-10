#pragma once
#include"Renderer.h"
#include<iostream>

void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}
 bool GLLogCall(const char* function, const char* file, int line) {

	while (GLenum error = glGetError())
	{
		std::cout << error << "  ::  " << function << "  >>  " << file << "  ::  " << line << "\n";
		//return false;
	}
	return true;
}

 void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& s) const
 {
	 va.Bind();
	 ib.Bind();
	 s.Bind();

	 GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
 }
 
 void Renderer::Clear() const
 {
	 GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
 }
