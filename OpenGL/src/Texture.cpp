#include <iostream>
#include <string>
#include "Texture.h"

Texture::Texture(const std::string& filepath, TextureType type, bool mipmaps)
	:m_generateMipmaps(mipmaps), m_textureType(type)
{
	if (m_generateMipmaps)
	{
		m_minFilter = TextureFilter::LINEAR_MIPMAP_LINEAR;
	}

	if (type == TextureType::T2D)
	{
		CreateTexture2D(filepath);
	}
	
	// TODO: Add Texture 1D and 3D creation
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_RendererId);
}

void Texture::CreateTexture2D(const std::string& filepath)
{
	glGenTextures(1, &m_RendererId);

	stbi_set_flip_vertically_on_load(true);

	int width, height, channels;
	unsigned char* textureData = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
	if (textureData == NULL)
	{
		std::cout << "Error on loading texture" << std::endl;

	}

	m_width = width;
	m_height = height;

	GLenum internalFormat = 0, dataFormat = 0;
	if (channels == 4)
	{
		internalFormat = GL_RGBA8;
		dataFormat = GL_RGBA;
	}
	else if (channels == 3)
	{
		internalFormat = GL_RGB8;
		dataFormat = GL_RGB;
	}

	m_internalFormat = internalFormat;
	m_dataFormat = dataFormat;

	Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_RendererId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (unsigned int)m_wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (unsigned int)m_wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (unsigned int)m_minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (unsigned int)m_maxFilter);

	glTexImage2D((unsigned int)m_textureType, 0, m_internalFormat, m_width, m_height, 0, m_dataFormat, GL_UNSIGNED_BYTE, textureData);
	if (m_generateMipmaps)
	{
		glGenerateMipmap((unsigned int)m_textureType);
	}

	Unbind();

	stbi_image_free(textureData);
}

void Texture::Bind(int unit)
{
	if (unit >= 0)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
	}

	glBindTexture((unsigned int)m_textureType, m_RendererId);
}

void Texture::Unbind()
{
	glBindTexture((unsigned int)m_textureType, 0);
}

void Texture::SetWrap(TextureWrap wrapMode)
{
	m_wrapS = wrapMode;
	m_wrapT = wrapMode;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (unsigned int)m_wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (unsigned int)m_wrapT);
}

void Texture::SetMinFilter(TextureFilter minFilter)
{
	m_minFilter = minFilter;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (unsigned int)m_minFilter);
}

void Texture::SetMaxFilter(TextureFilter maxFilter)
{	
	m_maxFilter = maxFilter;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (unsigned int)m_maxFilter);
}

void Texture::SetFilters(TextureFilter minFilter, TextureFilter maxFilter)
{
	m_minFilter = minFilter;
	m_maxFilter = maxFilter;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (unsigned int)m_minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (unsigned int)m_maxFilter);
}
