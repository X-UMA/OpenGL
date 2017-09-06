#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

namespace Mesh {
	class Mesh;
	class Buffer;
	//メッシュデータポインタ型
	typedef std::shared_ptr<Mesh> MeshPtr;
	//メッシュバッファポインタ型
	typedef std::shared_ptr<Buffer> BufferPtr;

	/**
	* マテリアル構造体
	*/
	struct Material {
		//インデックスのデータの型
		GLenum type;
		//描画するインデックス数
		GLsizei size;
		//描画開始インデックスのバイトオフセット
		GLvoid* offset;
		//インデックス0といmなされる頂点配列内の位置
		GLint baseVertex;
		//マテリアルの色
		glm::vec4 color;
	};

	/**
	* メッシュ
	*/
	class Mesh {
		friend class Buffer;
	public:
		const std::string& Name() const { return name; }
		void Draw(const BufferPtr& buffer) const;

	private:
		Mesh() = default;
		Mesh(const std::string& n, size_t begin, size_t end);
		Mesh(const Mesh&) = default;
		~Mesh() = default;
		Mesh& operator=(const Mesh&) = default;

	private:
		//メッシュデータ名
		std::string name;
		//テクスチャ名のリスト
		std::vector<std::string> texturelist;
		//描画するマテリアルの先頭インデックス
		size_t beginMaterial = 0;
		//描画するマテリアルの終端インデックス
		size_t endMaterial = 0;
	};

	/**
	* メッシュバッファ
	*/
	class Buffer {
	public:
		static BufferPtr Create(int vboSize, int iboSize);

		bool LoadMeshFromFile(const char* filename);
		const MeshPtr& GetMesh(const char* name)const;
		const Material& GetMaterial(size_t index)const;
		void BindVAO() const;

	private:
		Buffer() = default;
		~Buffer();
		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

	private:
		//モデルの頂点データを格納するVBO
		GLuint vbo = 0;
		//モデルのインデックスデータを格納するIBO
		GLuint ibo = 0;
		//モデル用VAO
		GLuint vao = 0;
		//読み込み済み頂点データの終端
		GLintptr vboEnd = 0;
		//読み込み済みインデックスデータの終端
		GLintptr iboEnd = 0;
		//マテリアルリスト
		std::vector<Material> materialList;
		//メッシュリスト
		std::unordered_map<std::string, MeshPtr>meshList;
	};
}