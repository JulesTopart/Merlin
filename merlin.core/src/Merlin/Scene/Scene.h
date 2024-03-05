#pragma once
#include <vector>

#include "Merlin/Graphics/RenderableObject.h"
#include "Merlin/Scene/Model.h"
#include "Merlin/Scene/SkyBox.h"
#include "Merlin/Scene/Light.h"
#include "Merlin/Scene/TransformObject.h"
#include "Merlin/Scene/deprecated_ParticleSystem.h"
#include "Camera.h"

namespace Merlin {

	class Scene : public RenderableObject {
		Shared<RenderableObject> _rootNode = nullptr;
		Shared<RenderableObject> _currentNode = nullptr;
		Shared<Camera> _camera = nullptr;

	public:
		Scene(std::string name = "scene");
		~Scene() {};

		void add(Shared<Model>);
		void add(Shared<deprecated_GenericParticleSystem>);
		void add(Shared<Light>);
		void add(Shared<SkyBox>);
		void add(Shared<TransformObject> t);

		void setCamera(Shared<Camera> camera);
		Camera& camera();

		const std::list<Shared<RenderableObject>>& nodes() const;

		inline static Shared<Scene> create() { return createShared<Scene>(); };
	};
	
}