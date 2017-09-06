#pragma once
/**
* @file Shader.h
*/
#include <GL/glew.h>
#include <string>
#include <memory>

namespace Shader {

	class Program;
	//プログラムオブジェクトポインタ型
	typedef std::shared_ptr<Program> ProgramPtr; 

	/**
	* シェーダープログラムクラス
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
		//プログラムオブジェクト
		GLuint program = 0;
		//サンプラーの位置
		GLint samplerLocation = -1;
		//サンプラー数
		int samplerConut = 0;
		//プログラム名
		std::string name;
	};

	GLuint CreateProgramFromFile(const char* vsFilename, const char* fsFilename);

}

