#pragma once
/**
* @file Shader.h
*/
#include <GL/glew.h>
#include <string>
#include <memory>

namespace Shader {

	class Program;
	//�v���O�����I�u�W�F�N�g�|�C���^�^
	typedef std::shared_ptr<Program> ProgramPtr; 

	/**
	* �V�F�[�_�[�v���O�����N���X
	*/
	class Program {
	public:
		static ProgramPtr Create(const char* vsFilename, const char* fsFilename);

		bool UniformBlockBinding(const char* bloackName, GLuint bindingPoint);
		void UseProgram();
		void BindTexture(GLenum unit, GLenum type, GLuint texture);

	private:
		Program() = default;
		~Program();
		Program(const Program&) = delete;
		Program& operator=(const Program&) = delete;

	private:
		//�v���O�����I�u�W�F�N�g
		GLuint program = 0;
		//�T���v���[�̈ʒu
		GLint samplerLocation = -1;
		//�T���v���[��
		int samplerConut = 0;
		//�v���O������
		std::string name;
	};

	GLuint CreateProgramFromFile(const char* vsFilename, const char* fsFilename);

}

