#pragma once
/**
* @file Texture.h
*/
#include <GL/glew.h>
#include <memory>

class Texture;
typedef std::shared_ptr<Texture> TexurePtr; ///<テクスチャポインタ

/**
* テクスチャクラス
*/
class Texture {
public:
	static TexurePtr Create(
		int width, int height, GLenum iformat, GLenum format, const void* data);

	static TexurePtr LoadFromFile(const char*);

	GLuint Id() const { return texId; }
	GLsizei Width() const { return width; }
	GLsizei Height() const { return height; }

private:
	Texture();
	~Texture();
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	GLuint texId;
	int width;
	int height;

};