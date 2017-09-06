/**
* @file Font.cpp
*/
#include "Font.h"
#include "GameEngine.h"
#include <memory>
#include <iostream>
#include <stdio.h>

/**
* �t�H���g�`��@�\���i�[���閼�O���
*/
namespace Font {
	/**
	* �t�H���g�p���_�f�[�^�^
	*/
	struct Vertex {
		vec2 position;
		u16vec2 uv;
		u8vec4 color;
	};

	/**
	* �t�H���g�`��I�u�W�F�N�g������������
	*
	* @param maxChar	�ő�`�敶����
	* @param screen		�`���̃X�N���[���̑傫��
	*
	* @retval true		����������
	* @retval false		���������s
	*/
	bool Renderer::Init(size_t maxChar, const vec2& screen) {
		if (maxChar > (USHRT_MAX + 1) / 4) {
			std::cerr << "WARNING : " << maxChar << "�͐ݒ�\�ȍő啶�����𒴂��Ă��܂�" << std::endl;
			maxChar = (USHRT_MAX + 1) / 4;
		}
		vboCapacity = static_cast<GLsizei>(4 * maxChar);
		vbo.Init(GL_ARRAY_BUFFER, sizeof(Vertex) * vboCapacity, nullptr, GL_STREAM_DRAW); {
			std::vector<GLushort> tmp;
			tmp.resize(maxChar * 6);
			GLushort* p = tmp.data();
			for (GLushort i = 0; i < maxChar * 4; i += 4) {
				for (GLushort n: { 0, 1, 2, 2, 3, 0 }) {
					*(p++) = i + n;
				}
			}
			ibo.Init(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 6 * maxChar, tmp.data(),
				GL_STATIC_DRAW);
		}
		vao.Init(vbo.Id(), ibo.Id());
		vao.Bind();
		vao.VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			offsetof(Vertex, position));
		vao.VertexAttribPointer(1, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(Vertex),
			offsetof(Vertex, uv));
		vao.VertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex),
			offsetof(Vertex, color));
		vao.Unbind();

		progFont = Shader::Program::Create("Res/Font.vert", "Res/Font.frag");
		if (!progFont) {
			return false;
		}

		//reciprocalScreenSize = 2.0f / screen;
		screenSize = screen;
		return true;
	}
	
	/**
	* �t�H���g�t�@�C����ǂݍ���
	*
	* @param filename	�t�H���g�t�@�C����
	* 
	* @retval true	�ǂݍ��ݐ���
	* @retval false	�ǂݍ��ݎ��s
	*/
	bool Renderer::LoadFromFile(const char* filename) {
		const std::unique_ptr<FILE, decltype(&fclose) >fp(fopen(filename, "r"), fclose);
		if (!fp) {
			std::cerr << "ERROR : '" << filename << "' �̃I�[�v���Ɏ��s" << std::endl;
			return false;
		}

		int line = 1; ///< �t�H���g�t�@�C���̏������̍s
		int ret = fscanf(fp.get(),
			"info face=%*s size=%*d bold=%*d italic=%*d charset=%*s unicode=%*d"
			" stretchH=%*d smooth=%*d aa=%*d padding=%*d,%*d,%*d,%*d spacing=%*d,%*d");
		++line;

		vec2 scale;
		ret = fscanf(fp.get(),
			" common lineHeight=%*d base=%*d scaleW=%f scaleH=%f pages=%*d packed=%*d",
			&scale.x, &scale.y);
		if (ret < 2) {
			std::cerr << "ERROR : '" << filename << "' �̓ǂݍ��݂Ɏ��s(line=" << line << ")" << std::endl;
			return false;
		}
		const vec2 reciprocalScale(1.0f / scale);
		screenScale = scale / screenSize;
		++line;

		char tex[128];
		ret = fscanf(fp.get(), " page id=%*d file=%127s", tex);
		if (ret < 1) {
			std::cerr << "ERROR : '" << filename << "' �̓ǂݍ��݂Ɏ��s(line=" << line << ")" << std::endl;
			return false;
		}
		texFilename.assign(tex + 1, tex + strlen(tex) - 1);
		++line;

		int charCount;
		ret = fscanf(fp.get(), " chars count=%d", &charCount);
		if (ret < 1) {
			std::cerr << "ERROR : '" << filename << "' �ǂݍ��݂Ɏ��s(line=" << line << ")" << std::endl;
			return false;
		}
		++line;

		fontList.resize(128);
		for (int i = 0; i < charCount; ++i) {
			FontInfo font;
			ret = fscanf(fp.get(),
				" char id=%d x=%f y=%f width=%f height=%f xoffset=%f yoffset=%f xadvance=%f"
				" page=%*d chnl=%*d", &font.id, &font.uv.x, &font.uv.y, &font.size.x, &font.size.y, &font.offset.x, &font.offset.y, &font.xadvance);
			if (ret < 8) {
				std::cerr << "ERROR : '" << filename << "'�ǂݍ��݂Ɏ��s(line=" << line << ")" << std::endl;
				return false;
			}
			
			font.uv *= reciprocalScale;
			font.size *= reciprocalScale;
			font.offset *= reciprocalScale;
			font.xadvance *= reciprocalScale.x;
			font.uv.y = 1 - font.uv.y - font.size.y;
			font.offset.y *= -1;

			if (font.id < 128) {
				fontList[font.id] = font;
			}
			++line;
		}

		//�e�N�X�`����ǂݍ���
		if (!GameEngine::Instance().LoadTextureFromFile(texFilename.c_str())) {
			return false;
		}
		return true;
	}
	
	/**
	* �����F��ݒ肷��
	*
	* @param c �����F
	*/
	void Renderer::Color(const vec4& c) {
		color = clamp(c, 0.0f, 1.0f) * 255.0f;
	}

	/**
	* �����F���擾����
	*
	* @retrun �����F
	*/
	vec4 Renderer::Color() const {
		return vec4(color) * (1.0f / 255.0f);
	}

	/**
	* �������ǉ�����
	*
	* @param position	�\���J�n���W
	* @param str		�ǉ����镶����
	*
	* @retval true	�ǉ�����
	* @retval false	�ǉ����s
	*/
	bool Renderer::AddString(const  vec2& position, const char* str) {
		Vertex* p = pVBO + vboSize;
		vec2 pos = position;
		for (const char* itr = str; *itr; ++itr) {
			if (vboSize + 4 > vboCapacity) {
				break;
			}
			const FontInfo& font = fontList[*itr];
			if (font.id >= 0 && font.size.x && font.size.y) {
				const vec2 size = font.size * screenScale * scale;
				const vec2 offsetedPos = pos + font.offset * scale;
				p[0].position = offsetedPos + glm::vec2(0, -size.y);
				p[0].uv = font.uv * 65535.0f;
				p[0].color = color;
				p[1].position = offsetedPos + glm::vec2(size.x, -size.y);
				p[1].uv = (font.uv + vec2(font.size.x, 0)) * 65535.0f;
				p[1].color = color;
				p[2].position = offsetedPos + glm::vec2(size.x, 0);
				p[2].uv = (font.uv + font.size) * 65535.0f;
				p[2].color = color;
				p[3].position = offsetedPos;
				p[3].uv = (font.uv + vec2(0, font.size.y)) * 65535.0f;
				p[3].color = color;
				p += 4;
				vboSize += 4;
			}
			pos.x += font.xadvance * screenScale.x * scale.x;
		}
		return true;
	}

	/**
	* VBO ���V�X�e���������Ƀ}�b�s���O����
	*/
	void Renderer::MapBuffer() {
		if (pVBO) {
			return;
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo.Id());
		pVBO = static_cast<Vertex*>(glMapBufferRange(GL_ARRAY_BUFFER, 0,
			sizeof(Vertex) * vboCapacity, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		vboSize = 0;
	}

	/**
	* VBO �̃}�b�s���O����������
	*/
	void Renderer::UnmapBuffer() {
		if (!pVBO || vboSize == 0) {
			return;
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo.Id());
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		pVBO = nullptr;
	}

	/**
	* �t�H���g��`�悷��
	*/
	void Renderer::Draw() const {
		if (vboSize == 0) {
			return;
		}
		vao.Bind();
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		progFont->UseProgram();
		progFont->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D,
			GameEngine::Instance().GetTexture(texFilename.c_str())->Id());
		glDrawElements(GL_TRIANGLES, (vboSize / 4) * 6, GL_UNSIGNED_SHORT, 0);
		vao.Unbind();
	}
}