/**
* @file Font.h
*/
#pragma once

#include <GL/glew.h>
#include "BufferObject.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <vector>
#include <string>

using namespace glm;

namespace Font {
	struct Vertex;

	/// �t�H���g���
	struct FontInfo {
		int id = -1;		///< �����R�[�h
		vec2 uv;			///< �t�H���g�摜�̍���e�N�X�`�����W
		vec2 size;			///< �t�H���g�摜�̃e�N�Z���T�C�Y
		vec2 offset;		///< �\���ʒu�����炷����
		float xadvance = 0;	///< �J�[�\����i�߂鋗��
	};

	/**
	* �r�b�g�}�b�v�t�H���g�`��N���X
	*/
	class Renderer {
	public:
		Renderer() = default;
		~Renderer() = default;
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		bool Init(size_t maxChar, const vec2& ss);
		bool LoadFromFile(const char* filename);

		void Scale(const vec2& s) { scale = s; }
		const vec2& Scale() const { return scale; }
		void Color(const vec4& c);
		vec4 Color() const;

		void MapBuffer();
		bool AddString(const vec2& position, const char* str);
		void UnmapBuffer();
		void Draw() const;

	private:
		BufferObject vbo;
		BufferObject ibo;
		VertexArrayObject vao;
		GLsizei vboCapacity = 0;		///< VBO�Ɋi�[�\�ȍő咷�_��
		std::vector<FontInfo> fontList;	///< �t�H���g�ʒu��񃊃X�g
		std::string texFilename;		///< �t�H���g�e�N�X�`���t�@�C����
		Shader::ProgramPtr progFont;	///< �t�H���g�`��p�V�F�[�_�v���O����.0

		vec2 screenSize;
		vec2 screenScale;

		vec2 scale = vec2(1, 1);					///< �t�H���g��`�悷�鎞�̊g�嗦
		u8vec4 color = u8vec4(255, 255, 255, 255);	///< �t�H���g��`�悷�鎞�̐F
		GLsizei vboSize = 0;						///< VBO�Ɋi�[����Ă��钸�_��
		Vertex* pVBO = nullptr;						///< VBO�ւ̃|�C���^
	};
}