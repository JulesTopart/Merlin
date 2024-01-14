#pragma once
#include <vector>

#include "Merlin/Graphics/RenderableObject.h"
#include "Merlin/Scene/Model.h"
#include "Merlin/Scene/SkyBox.h"
#include "Merlin/Scene/Light.h"
#include "Merlin/Scene/TransformObject.h"
#include "Merlin/Scene/deprecated_ParticleSystem.h"
#include "Camera.h"

using namespace Merlin::Tensor;

namespace Merlin::Graphics {

	class Scene : public RenderableObject {
		Shared<RenderableObject> _rootNode = nullptr;
		Shared<RenderableObject> _currentNode = nullptr;
		Shared<Camera> _camera = nullptr;

	public:
		Scene(std::string name = "scene");
		~Scene() {};

		void Add(Shared<Model>);
		void Add(Shared<deprecated_GenericParticleSystem>);
		void Add(Shared<Light>);
		void Add(Shared<SkyBox>);
		void Add(Shared<TransformObject> t);

		void SetCamera(Shared<Camera> camera);
		Camera& camera();

		const std::list<Shared<RenderableObject>>& nodes() const;

		inline static Shared<Scene> Create() { return CreateShared<Scene>(); };
	};
	
}