/**
* @file Uniform.h
*/
#pragma once
#include <glm/glm.hpp>


namespace Uniform {
	//���_�V�F�[�_�̃p�����[�^�^
	struct VertexData {
		glm::mat4 matMVP;
		glm::mat4 matModel;
		glm::mat3x4 matNormal;
		glm::vec4 color;
	};

	//���C�g�f�[�^(�_����)
	struct PointLight {
		//���W(���[���h���W)
		glm::vec4 position;
		//���邳
		glm::vec4 color;
	};

	//���C�g�̐�
	const int maxLightCount = 4;

	//���C�e�B���O�p�����[�^
	struct LightData {
		//����
		glm::vec4 ambientColor;
		//���C�g�̃��X�g
		PointLight light[maxLightCount];
	};

	/**
	* �|�X�g�G�t�F�N�g�f�[�^
	*/
	struct PostEffectData {
		//�F�ϊ��s��
		glm::mat4x4 matColor;
	};
}