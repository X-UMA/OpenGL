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
* バイト列から数値を復元する
*
* @param p		バイト列へのポインタ
* @param offset	数値のオフセット
* @param size	数値のバイト数(1～4)
*
* @return 復元した数値
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
* コンストラクタ
*/
Texture::Texture() : texId(0), width(0), height(0) {

}

/**
* デストラクタ
*/
Texture::~Texture() {
	if (texId) {
		glDeleteTextures(1, &texId);
	}
}

/**
* 2D テクスチャを作成する
* 
* @param width		テクスチャの幅(ピクセル数)
* @param height		テクスチャの高さ(ピクセル数)
* @param iformat	テクスチャのデータ形式
* @param format		アクセスする要素
* @param data		テクスチャデータへのポインタ
*
* @return	作成に成功した場合はテクスチャポインタを返す
*			失敗した場合はnullptrを返す
*/
TexurePtr Texture::Create(
	int width, int height, GLenum iformat, GLenum format, const void* data) {
	//Impl関数はVS2015だと怒られるので
	struct Impl : Texture{
		//これを書く
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
		std::cerr << "ERROR テクスチャ作成に失敗： 0x" << std::hex << result << std::endl;
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
* ファイルから2Dテクスチャを読み込む
*
* @param filename ファイル名
*
* @retrun	作成に成功した場合はテクスチャポインタを返す
*			失敗した場合はnullptrを返す
*/
TexurePtr Texture::LoadFromFile(const char* filename) {
	//ファイルを開く
	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		return{};
	}

	//stat関数でファイルサイズを取得する
	struct stat st;
	if (stat(filename, &st)) {
		return{};
	}

	//BMPファイルの情報を保持できるサイズかを調べる
	const size_t bmpFileHeaderSize = 14;
	const size_t windowsV1HeaderSize = 40;
	if (st.st_size < bmpFileHeaderSize + windowsV1HeaderSize) {
		return{};
	}

	//読み込み
	std::vector<uint8_t>buf;
	buf.resize(st.st_size);
	const size_t readSize = fread(buf.data(), 1, st.st_size, fp);
	fclose(fp);
	if (readSize != st.st_size) {
		return{};
	}

	//先頭2バイトを検査する
	const uint8_t* pHeader = buf.data();
	if (pHeader[0] != 'B' || pHeader[1] != 'M') {
		return{};
	}

	//詳細データを検査する
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

	//画像データが正しく格納されているかを検査する
	const size_t imageSize = widht* height * pixelBytes;
	if (buf.size() < offsetBytes + imageSize) {
		return{};
	}

	//テクスチャを作成する
	return Create(widht, height, GL_RGBA, GL_BGR, buf.data() + offsetBytes);
}

