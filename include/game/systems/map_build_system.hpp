#ifndef GAME_MAP_BUILD_SYSTEM_HPP
# define GAME_MAP_BUILD_SYSTEM_HPP

# include <string>

class Scene;

class MapBuildSystem
{
	public:
		bool build(Scene& scene, const std::string& mapPath);
};

#endif
