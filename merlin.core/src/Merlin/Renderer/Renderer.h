#pragma once
#include "Merlin/Renderer/Scene.h"
#include "Merlin/Renderer/Camera.h"


namespace Merlin::Renderer {
	class Renderer{
	public:
		Renderer();
		~Renderer();

		void SetCamera(const Camera& camera);
		void SetScene(const Scene& scene);

		void Render();


	private:
		Camera _camera;
		Scene _scene;
	};
}
