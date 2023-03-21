#pragma once
#include "Merlin/Scene/Scene.h"
#include "Merlin/Scene/Camera.h"


namespace Merlin::Graphics {
	class Renderer{
	public:
		Renderer();
		~Renderer();

		void Initialize();
		void Render(const Shared<Scene>& scene, const Camera& camera);

		void EnableTransparency();
		void EnableMultisampling();
		void DisableTransparency();
		void DisableMultisampling();

	};
}
