#include "render/texture.hpp"

#include <glad/glad.h>
#include "stb_image.h"

#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>

namespace
{
	void trim(std::string& text)
	{
		while (!text.empty() && std::isspace(static_cast<unsigned char>(text[text.size() - 1])))
			text.erase(text.size() - 1);
		std::size_t start = 0;
		while (start < text.size() && std::isspace(static_cast<unsigned char>(text[start])))
			++start;
		if (start > 0)
			text.erase(0, start);
	}
}

Texture::Texture()
	: id(0)
{}

Texture::~Texture()
{
	if (this->id)
		glDeleteTextures(1, &this->id);
}

bool Texture::loadFromFile(const std::string& path)
{
	glGenTextures(1, &this->id);
	glBindTexture(GL_TEXTURE_2D, this->id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);
	int width = 0;
	int height = 0;
	int nrChannels = 0;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if (!data)
	{
		std::cerr << "Falha ao carregar textura: " << path << std::endl;
		return false;
	}

	GLenum format = GL_RGB;
	if (nrChannels == 1)
		format = GL_RED;
	else if (nrChannels == 4)
		format = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	return true;
}

void Texture::bind() const
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->id);
}

bool TextureCatalog::load(const std::string& catalogPath)
{
	std::ifstream file(catalogPath.c_str());
	if (!file)
	{
		std::cerr << "Falha ao abrir catálogo de texturas: " << catalogPath << std::endl;
		return false;
	}

	std::string line;
	int lineNo = 0;
	while (std::getline(file, line))
	{
		++lineNo;
		trim(line);
		if (line.empty() || line[0] == '#')
			continue;
		if (line.size() >= 2 && line[0] == '/' && line[1] == '/')
			continue;

		std::istringstream iss(line);
		int textureId = 0;
		if (!(iss >> textureId))
		{
			std::cerr << "Id inválido em " << catalogPath << ":" << lineNo << std::endl;
			return false;
		}

		std::string path;
		std::getline(iss, path);
		trim(path);
		if (path.empty())
		{
			std::cerr << "Caminho vazio em " << catalogPath << ":" << lineNo << std::endl;
			return false;
		}
		if (this->textures.find(textureId) != this->textures.end())
		{
			std::cerr << "Id de textura repetido: " << textureId << std::endl;
			return false;
		}

		std::unique_ptr<Texture> texture(new Texture());
		if (!texture->loadFromFile(path))
			return false;
		this->textures[textureId] = std::move(texture);
	}

	if (this->textures.empty())
	{
		std::cerr << "Catálogo de texturas vazio: " << catalogPath << std::endl;
		return false;
	}
	return true;
}

Texture* TextureCatalog::find(int textureId) const
{
	std::map<int, std::unique_ptr<Texture> >::const_iterator it = this->textures.find(textureId);
	if (it == this->textures.end())
		return 0;
	return it->second.get();
}
