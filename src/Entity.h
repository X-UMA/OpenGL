#pragma once
/**
* @file Entity.h
*/
#include <GL/glew.h>
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "UniformBuffer.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include <functional>
#include <vector>

namespace Entity{
	class Entity;
	class Buffer;
	typedef std::shared_ptr<Buffer> BufferPtr;	///< �G���e�B�e�B�o�b�t�@�|�C���^�^

	/// �Փˉ����n���h���^
	typedef std::function<void(Entity&, Entity&)> CollisionHandlerType;

	static const int maxGroupId = 31; ///< �O���[�vID�̍ő吔

	/**
	* �Փ˔���`��
	*/
	struct CollisionData {
		glm::vec3 min;
		glm::vec3 max;
	};

	/**
	* �G���e�B�e�B
	*/
	class Entity {
		friend class Buffer;

	public:
		///��ԍX�V�֐��^
		typedef std::function<void(Entity&, double)>UpdateFuncType;

		void Position(const glm::vec3& v) { position = v; }
		const glm::vec3& Position() const { return position; }
		void Rotation(const glm::quat& q) { rotation = q; }
		const glm::quat& Rotation() const { return rotation; }
		void Scale(const glm::vec3& v) { scale = v; }
		const glm::vec3& Scale() const { return scale; }		
		void Color(const glm::vec4& v) { color = v; }
		const glm::vec4& Color() const { return color; }
		void Velocity(const glm::vec3& v) { velocity = v; }
		const glm::vec3& Velocity()const { return velocity; }
		void UpdateFunc(const UpdateFuncType& func) { updateFunc = func; }
		const UpdateFuncType& UpdateFunc() const { return updateFunc; }
		void Collision(const CollisionData& c) { colLocal = c; }
		const CollisionData& Collision() const { return colLocal; }

		glm::mat4 CalcModelMatrix() const;
		int GroupId() const { return groupId; }
		void Destroy();

	private:
		Entity() = default;
		~Entity() = default;
		Entity(const Entity&) = default;
		Entity& operator= (const Entity&) = default;

	private:
		//�O���[�vID
		int groupId = -1;
		//��������Buffer�N���X�ւ̃|�C���^
		Buffer* pBuffer = nullptr;
		//���W
		glm::vec3 position;
		//�g�嗦
		glm::vec3 scale = glm::vec3(1, 1, 1);
		//��]
		glm::quat rotation;
		//���x
		glm::vec3 velocity;
		//�F
		glm::vec4 color = glm::vec4(1, 1, 1, 1);
		//��ԍX�V�֐�
		UpdateFuncType updateFunc;
		//�G���e�B�e�B��`�悷�鎞�Ɏg���郁�b�V���f�[�^
		Mesh::MeshPtr mesh;
		//�G���e�B�e�B��`�悷�鎞�Ɏg����e�N�X�`��
		TexurePtr texture;
		//�G���e�B�e�B��`�悷�鎞�Ɏg����V�F�[�_
		Shader::ProgramPtr program;
		//UBO�̃G���e�B�e�B�p�̈�ւ̃o�C�g�I�t�Z�b�g
		GLintptr uboOffset;
		//���[�J�����W�n�̏Փˌ`��
		CollisionData colLocal;
		//���[���h���W�n�̏Փˌ`��
		CollisionData ColWorld;
		//�A�N�e�B�u�ȃG���e�B�e�B�Ȃ�true,��A�N�e�B�u�Ȃ�falae
		bool isActive = false;
	};

	/**
	* �G���e�B�e�B�o�b�t�@
	*/
	class Buffer {
	public:
		static BufferPtr Create(size_t maxEntityConst, GLsizeiptr ubSizePerEntity,
			int bindingPoint, const char* name);

		Entity* AddEntity(int groupId, const glm::vec3& pos, const Mesh::MeshPtr& m, const TexurePtr& t,
			const Shader::ProgramPtr& p, Entity::UpdateFuncType func);
		void RemoveEntity(Entity* entity);
		void Update(double delta, const glm::mat4& matView, const glm::mat4& matProj);
		void Draw(const Mesh::BufferPtr& meshBuffer)const;

		void CollisionHandler(int gid0, int gid1, CollisionHandlerType handler);
		const CollisionHandlerType& CollisionHandler(int gid0, int gid1)const;
		void ClearCollisionHandlerList();

	private:
		Buffer() = default;
		~Buffer() = default;
		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

	private:
		//�G���e�B�e�B�p�����N���X�g
		struct Link {
			void Insert(Link* e);
			void Remove();
			Link* prev = this;
			Link* next = this;
		};
		//�����N�t���G���e�B�e�B
		struct LinkEntity : public Link, public Entity {};
		//�����N�t���G���e�B�e�B�z��̍폜�p�֐��I�u�W�F�N�g
		struct EntityArrayDeleter { void operator()(LinkEntity* p) { delete[] p; } };

		//�G���e�B�e�B�̔z��
		std::unique_ptr<LinkEntity[], EntityArrayDeleter>buffer;
		//�G���e�B�e�B�̑���
		size_t bufferSize;
		//���g�p�̃G���e�B�e�B�̃����N���X�g
		Link freeList;
		//�g�p���̃G���e�B�e�B�̃����N���X�g
		Link activeList[maxGroupId + 1];
		//�e�G���e�B�e�B���g����Uniform Buffer�̃o�C�g��
		GLsizeiptr ubSizePerEntity;
		//�G���e�B�e�B�p UBO
		UniformBufferPtr ubo;
		//Update��RemoveEntity�̑��ݍ�p�ɑΉ����邽�߂̃C�e���[�^
		Link* itrUpdate = nullptr;
		Link* itrUpdateRhs = nullptr;

		struct CollisionHandlerInfo {
			int groupId[2];
			CollisionHandlerType handler;
		};
		std::vector<CollisionHandlerInfo> collisionhandlerList;
	};
}