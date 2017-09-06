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
	//���b�V���f�[�^�|�C���^�^
	typedef std::shared_ptr<Mesh> MeshPtr;
	//���b�V���o�b�t�@�|�C���^�^
	typedef std::shared_ptr<Buffer> BufferPtr;

	/**
	* �}�e���A���\����
	*/
	struct Material {
		//�C���f�b�N�X�̃f�[�^�̌^
		GLenum type;
		//�`�悷��C���f�b�N�X��
		GLsizei size;
		//�`��J�n�C���f�b�N�X�̃o�C�g�I�t�Z�b�g
		GLvoid* offset;
		//�C���f�b�N�X0�Ƃ�m�Ȃ���钸�_�z����̈ʒu
		GLint baseVertex;
		//�}�e���A���̐F
		glm::vec4 color;
	};

	/**
	* ���b�V��
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
		//���b�V���f�[�^��
		std::string name;
		//�e�N�X�`�����̃��X�g
		std::vector<std::string> texturelist;
		//�`�悷��}�e���A���̐擪�C���f�b�N�X
		size_t beginMaterial = 0;
		//�`�悷��}�e���A���̏I�[�C���f�b�N�X
		size_t endMaterial = 0;
	};

	/**
	* ���b�V���o�b�t�@
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
		//���f���̒��_�f�[�^���i�[����VBO
		GLuint vbo = 0;
		//���f���̃C���f�b�N�X�f�[�^���i�[����IBO
		GLuint ibo = 0;
		//���f���pVAO
		GLuint vao = 0;
		//�ǂݍ��ݍςݒ��_�f�[�^�̏I�[
		GLintptr vboEnd = 0;
		//�ǂݍ��ݍς݃C���f�b�N�X�f�[�^�̏I�[
		GLintptr iboEnd = 0;
		//�}�e���A�����X�g
		std::vector<Material> materialList;
		//���b�V�����X�g
		std::unordered_map<std::string, MeshPtr>meshList;
	};
}