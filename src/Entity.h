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
	typedef std::shared_ptr<Buffer> BufferPtr;	///< エンティティバッファポインタ型

	/// 衝突解決ハンドラ型
	typedef std::function<void(Entity&, Entity&)> CollisionHandlerType;

	static const int maxGroupId = 31; ///< グループIDの最大数

	/**
	* 衝突判定形状
	*/
	struct CollisionData {
		glm::vec3 min;
		glm::vec3 max;
	};

	/**
	* エンティティ
	*/
	class Entity {
		friend class Buffer;

	public:
		///状態更新関数型
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
		//グループID
		int groupId = -1;
		//生成元のBufferクラスへのポインタ
		Buffer* pBuffer = nullptr;
		//座標
		glm::vec3 position;
		//拡大率
		glm::vec3 scale = glm::vec3(1, 1, 1);
		//回転
		glm::quat rotation;
		//速度
		glm::vec3 velocity;
		//色
		glm::vec4 color = glm::vec4(1, 1, 1, 1);
		//状態更新関数
		UpdateFuncType updateFunc;
		//エンティティを描画する時に使われるメッシュデータ
		Mesh::MeshPtr mesh;
		//エンティティを描画する時に使われるテクスチャ
		TexurePtr texture;
		//エンティティを描画する時に使われるシェーダ
		Shader::ProgramPtr program;
		//UBOのエンティティ用領域へのバイトオフセット
		GLintptr uboOffset;
		//ローカル座標系の衝突形状
		CollisionData colLocal;
		//ワールド座標系の衝突形状
		CollisionData ColWorld;
		//アクティブなエンティティならtrue,非アクティブならfalae
		bool isActive = false;
	};

	/**
	* エンティティバッファ
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
		//エンティティ用リンクリスト
		struct Link {
			void Insert(Link* e);
			void Remove();
			Link* prev = this;
			Link* next = this;
		};
		//リンク付きエンティティ
		struct LinkEntity : public Link, public Entity {};
		//リンク付きエンティティ配列の削除用関数オブジェクト
		struct EntityArrayDeleter { void operator()(LinkEntity* p) { delete[] p; } };

		//エンティティの配列
		std::unique_ptr<LinkEntity[], EntityArrayDeleter>buffer;
		//エンティティの総数
		size_t bufferSize;
		//未使用のエンティティのリンクリスト
		Link freeList;
		//使用中のエンティティのリンクリスト
		Link activeList[maxGroupId + 1];
		//各エンティティが使えるUniform Bufferのバイト数
		GLsizeiptr ubSizePerEntity;
		//エンティティ用 UBO
		UniformBufferPtr ubo;
		//UpdateとRemoveEntityの相互作用に対応するためのイテレータ
		Link* itrUpdate = nullptr;
		Link* itrUpdateRhs = nullptr;

		struct CollisionHandlerInfo {
			int groupId[2];
			CollisionHandlerType handler;
		};
		std::vector<CollisionHandlerInfo> collisionhandlerList;
	};
}