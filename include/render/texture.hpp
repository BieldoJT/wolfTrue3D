#ifndef RENDER_TEXTURE_HPP
# define RENDER_TEXTURE_HPP

# include <map>
# include <memory>
# include <string>

class Texture
{
	private:
		unsigned int id;

	public:
		Texture();
		~Texture();

		bool loadFromFile(const std::string& path);
		void bind() const;
};

class TextureCatalog
{
	private:
		std::map<int, std::unique_ptr<Texture> > textures;

	public:
		bool load(const std::string& catalogPath);
		Texture* find(int textureId) const;
};

#endif
