#pragma once
#include "Texture.h"
#include <GL/glew.h>
#include <memory>

class OffscreenBuffer;
typedef std::shared_ptr<OffscreenBuffer> OffscreenBufferPtr;

/**
* オフスクリーンバッファ
*/
class OffscreenBuffer {
public:

	static OffscreenBufferPtr Create(int w, int h);
	GLuint GetFramebuffer() const { return framebuffer; }
	GLuint GetTextre() const { return tex->Id(); }

private:
	OffscreenBuffer() = default;
	OffscreenBuffer(const OffscreenBuffer&) = delete;
	OffscreenBuffer& operator=(const OffscreenBuffer&) = delete;
	~OffscreenBuffer();

private:
	TexurePtr tex;
	GLuint depthbuffer = 0;
	GLuint framebuffer = 0;
};