/**
* @file Uniform.h
*/
#pragma once
#include <glm/glm.hpp>


namespace Uniform {
	//頂点シェーダのパラメータ型
	struct VertexData {
		glm::mat4 matMVP;
		glm::mat4 matModel;
		glm::mat3x4 matNormal;
		glm::vec4 color;
	};

	//ライトデータ(点光源)
	struct PointLight {
		//座標(ワールド座標)
		glm::vec4 position;
		//明るさ
		glm::vec4 color;
	};

	//ライトの数
	const int maxLightCount = 4;

	//ライティングパラメータ
	struct LightData {
		//環境光
		glm::vec4 ambientColor;
		//ライトのリスト
		PointLight light[maxLightCount];
	};

	/**
	* ポストエフェクトデータ
	*/
	struct PostEffectData {
		//色変換行列
		glm::mat4x4 matColor;
	};
}