#include "render/texture.hpp"

#include <glad/glad.h>
#include "stb_image.h"
#include "map/catalog.hpp"

#include <iostream>
#include <vector>

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
	std::map<int, std::string> entries;
	std::vector<MapMessage> messages;
	bool ok = readTextureCatalog(catalogPath, entries, messages);
	printMapMessages(std::cerr, catalogPath, messages);
	if (!ok)
		return false;

	for (std::map<int, std::string>::const_iterator it = entries.begin(); it != entries.end(); ++it)
	{
		std::unique_ptr<Texture> texture(new Texture());
		if (!texture->loadFromFile(it->second))
			return false;
		this->textures[it->first] = std::move(texture);
	}
	return true;
}

std::set<int> TextureCatalog::ids() const
{
	std::set<int> result;
	for (std::map<int, std::unique_ptr<Texture> >::const_iterator it = this->textures.begin();
		it != this->textures.end(); ++it)
		result.insert(it->first);
	return result;
}

Texture* TextureCatalog::find(int textureId) const
{
	std::map<int, std::unique_ptr<Texture> >::const_iterator it = this->textures.find(textureId);
	if (it == this->textures.end())
		return 0;
	return it->second.get();
}
