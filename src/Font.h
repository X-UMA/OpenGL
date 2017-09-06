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

	/// フォント情報
	struct FontInfo {
		int id = -1;		///< 文字コード
		vec2 uv;			///< フォント画像の左上テクスチャ座標
		vec2 size;			///< フォント画像のテクセルサイズ
		vec2 offset;		///< 表示位置をずらす距離
		float xadvance = 0;	///< カーソルを進める距離
	};

	/**
	* ビットマップフォント描画クラス
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
		GLsizei vboCapacity = 0;		///< VBOに格納可能な最大長点数
		std::vector<FontInfo> fontList;	///< フォント位置情報リスト
		std::string texFilename;		///< フォントテクスチャファイル名
		Shader::ProgramPtr progFont;	///< フォント描画用シェーダプログラム.0

		vec2 screenSize;
		vec2 screenScale;

		vec2 scale = vec2(1, 1);					///< フォントを描画する時の拡大率
		u8vec4 color = u8vec4(255, 255, 255, 255);	///< フォントを描画する時の色
		GLsizei vboSize = 0;						///< VBOに格納されている頂点数
		Vertex* pVBO = nullptr;						///< VBOへのポインタ
	};
}