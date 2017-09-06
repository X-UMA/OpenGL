/**
* @file Texture.cpp
*/
#include "Texture.h"
#include <iostream>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

/**
* �o�C�g�񂩂琔�l�𕜌�����
*
* @param p		�o�C�g��ւ̃|�C���^
* @param offset	���l�̃I�t�Z�b�g
* @param size	���l�̃o�C�g��(1�`4)
*
* @return �����������l
*/
uint32_t Get(const uint8_t* p, size_t offset, size_t size) {
	uint32_t n = 0;
	p += offset;
	for (size_t i = 0; i < size; i++) {
		n += p[i] << (i * 8);
	}
	return n;
}

/**
* �R���X�g���N�^
*/
Texture::Texture() : texId(0), width(0), height(0) {

}

/**
* �f�X�g���N�^
*/
Texture::~Texture() {
	if (texId) {
		glDeleteTextures(1, &texId);
	}
}

/**
* 2D �e�N�X�`�����쐬����
* 
* @param width		�e�N�X�`���̕�(�s�N�Z����)
* @param height		�e�N�X�`���̍���(�s�N�Z����)
* @param iformat	�e�N�X�`���̃f�[�^�`��
* @param format		�A�N�Z�X����v�f
* @param data		�e�N�X�`���f�[�^�ւ̃|�C���^
*
* @return	�쐬�ɐ��������ꍇ�̓e�N�X�`���|�C���^��Ԃ�
*			���s�����ꍇ��nullptr��Ԃ�
*/
TexurePtr Texture::Create(
	int width, int height, GLenum iformat, GLenum format, const void* data) {
	//Impl�֐���VS2015���Ɠ{����̂�
	struct Impl : Texture{
		//���������
		Impl() {}
		~Impl() {}
	};
	TexurePtr p = std::make_shared<Impl>();

	p->width = width;
	p->height = height;
	glGenTextures(1, &p->texId);
	glBindTexture(GL_TEXTURE_2D, p->texId);
	glTexImage2D(
		GL_TEXTURE_2D, 0, iformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	const GLenum result = glGetError();
	if (result != GL_NO_ERROR) {
		std::cerr << "ERROR �e�N�X�`���쐬�Ɏ��s�F 0x" << std::hex << result << std::endl;
		return{};
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	return p;
}

/**
* �t�@�C������2D�e�N�X�`����ǂݍ���
*
* @param filename �t�@�C����
*
* @retrun	�쐬�ɐ��������ꍇ�̓e�N�X�`���|�C���^��Ԃ�
*			���s�����ꍇ��nullptr��Ԃ�
*/
TexurePtr Texture::LoadFromFile(const char* filename) {
	//�t�@�C�����J��
	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		return{};
	}

	//stat�֐��Ńt�@�C���T�C�Y���擾����
	struct stat st;
	if (stat(filename, &st)) {
		return{};
	}

	//BMP�t�@�C���̏���ێ��ł���T�C�Y���𒲂ׂ�
	const size_t bmpFileHeaderSize = 14;
	const size_t windowsV1HeaderSize = 40;
	if (st.st_size < bmpFileHeaderSize + windowsV1HeaderSize) {
		return{};
	}

	//�ǂݍ���
	std::vector<uint8_t>buf;
	buf.resize(st.st_size);
	const size_t readSize = fread(buf.data(), 1, st.st_size, fp);
	fclose(fp);
	if (readSize != st.st_size) {
		return{};
	}

	//�擪2�o�C�g����������
	const uint8_t* pHeader = buf.data();
	if (pHeader[0] != 'B' || pHeader[1] != 'M') {
		return{};
	}

	//�ڍ׃f�[�^����������
	const size_t offsetBytes = Get(pHeader, 10, 4);
	const uint32_t infoSize = Get(pHeader, 14, 4);
	const uint32_t widht = Get(pHeader, 18, 4);
	const uint32_t height = Get(pHeader, 22, 4);
	const uint32_t bitCount = Get(pHeader, 28, 2);
	const uint32_t compression = Get(pHeader, 30, 4);
	const size_t pixelBytes = bitCount / 8;
	if (infoSize != windowsV1HeaderSize ||
		bitCount != 24 ||
		compression ||
		(widht*pixelBytes) % 4) {
		printf("");
		return{};
	}

	//�摜�f�[�^���������i�[����Ă��邩����������
	const size_t imageSize = widht* height * pixelBytes;
	if (buf.size() < offsetBytes + imageSize) {
		return{};
	}

	//�e�N�X�`�����쐬����
	return Create(widht, height, GL_RGBA, GL_BGR, buf.data() + offsetBytes);
}

