#pragma once
/**
* @file UniformBuffer.h
*/
#include <GL/glew.h>
#include <string>
#include <memory>

class UniformBuffer;
//UBOポインタ型
typedef std::shared_ptr<UniformBuffer> UniformBufferPtr;

/**
* UBOクラス
*/
class UniformBuffer {
public:
	static UniformBufferPtr Create(
		GLsizeiptr size,GLuint binddingPoint,const char* name);
	bool BufferSubData(const GLvoid* data, GLintptr offset = 0, GLsizeiptr size = 0);
	void BindBufferRange(GLintptr offset, GLsizeiptr size) const;
	void* MapBuffer()const;
	void UnmapBuffer()const;
	GLsizeiptr Size() const { return size; }

private:
	UniformBuffer() = default;
	~UniformBuffer();
	UniformBuffer(const UniformBuffer&) = delete;
	UniformBuffer& operator = (const UniformBuffer&) = delete;

private:
	//Unifomr Buffer Object
	GLuint ubo = 0;
	//UBOのバイトサイズ
	GLsizeiptr size = 0;
	//UBO割当先バインディング・ポイント
	GLuint bindingPoint;
	//UBO名
	std::string name;
};