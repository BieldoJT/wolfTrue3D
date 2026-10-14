#ifndef GAME_CAMERA_SYSTEM_HPP
# define GAME_CAMERA_SYSTEM_HPP

class Scene;
class Input;

class CameraSystem
{
	private:
		bool ignoreFirstMouse;

	public:
		CameraSystem();
		void update(Scene& scene, const Input& input, float dt);
};

#endif
