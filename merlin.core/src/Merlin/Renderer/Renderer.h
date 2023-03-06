#pragma once
#include "Merlin/Scene/Scene.h"
#include "Merlin/Scene/Camera.h"


namespace Merlin::Renderer {
	class Renderer{
	public:
		Renderer();
		~Renderer();

		void SetCamera(const Shared<Camera>& camera);
		void SetScene(const Shared<Scene>& scene);

		void Initialize();
		void Render();

		void EnableTransparency();
		void EnableMultisampling();
		void DisableTransparency();
		void DisableMultisampling();


	private:
		Shared<Camera> _camera;
		Shared<Scene> _scene;
	};
}
