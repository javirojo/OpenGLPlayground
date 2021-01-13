#pragma once
#include <glad/glad.h>
#include <string>
#include "stb_image.h"

enum class TextureType {
	T1D = GL_TEXTURE_1D,
	T2D = GL_TEXTURE_2D,
	T3D = GL_TEXTURE_3D,
};

enum class TextureWrap {
	REPEAT = GL_REPEAT,
	MIRROR = GL_MIRRORED_REPEAT,
	CLAMP = GL_CLAMP_TO_EDGE,
	CLAMP_BORDER = GL_CLAMP_TO_BORDER

};

enum class TextureFilter {
	NEAREST = GL_NEAREST,
	LINEAR = GL_LINEAR,
	NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
	LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
	NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
};

class Texture
{
public:
	Texture(const std::string& filepath, TextureType type, bool mipmaps = true);
	~Texture();
	void Bind(int unit = 0);
	void Unbind();
	void SetWrap(TextureWrap wrapMode);
	void SetMinFilter(TextureFilter minFilter);
	void SetMaxFilter(TextureFilter maxFilter);
	void SetFilters(TextureFilter minFilter, TextureFilter maxFilter);

private:
	void CreateTexture2D(const std::string& filepath);
	// TODO: Add Texture 1D and 3D creation

	unsigned int m_RendererId = 0;
	TextureType m_textureType;

	bool m_generateMipmaps;
	unsigned int m_width = 0;
	unsigned int m_height = 0;
	unsigned int m_Depth = 0;
	unsigned int m_internalFormat = GL_RGBA;
	unsigned int m_dataFormat = GL_RGBA;
	TextureWrap m_wrapS = TextureWrap::REPEAT;
	TextureWrap m_wrapT = TextureWrap::REPEAT;
	TextureWrap m_wrapR = TextureWrap::REPEAT;
	TextureFilter m_minFilter = TextureFilter::LINEAR;
	TextureFilter m_maxFilter = TextureFilter::LINEAR;
};