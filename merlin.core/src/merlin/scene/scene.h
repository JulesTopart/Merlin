#pragma once
#include <vector>

#include "merlin/graphics/renderableObject.h"
#include "merlin/scene/model.h"
#include "merlin/scene/environment.h"
#include "merlin/scene/light.h"
#include "merlin/scene/transformObject.h"
#include "merlin/scene/deprecated_particleSystem.h"
#include "merlin/physics/particleSystem.h"
#include "camera.h"

namespace Merlin {

	class Scene : public RenderableObject {
		Shared<RenderableObject> _rootNode = nullptr;
		Shared<RenderableObject> _currentNode = nullptr;
		Shared<Camera> _camera = nullptr;

	public:
		Scene(std::string name = "scene");

		void add(Shared<Model>);
		void add(Shared<ParticleSystem>);
		void add(Shared<deprecated_GenericParticleSystem>);
		void add(Shared<Light>);
		void add(Shared<Environment>);
		void add(Shared<TransformObject> t);

		void setCamera(Shared<Camera> camera);
		Camera& camera();

		const std::list<Shared<RenderableObject>>& nodes() const;

		inline static Shared<Scene> create() { return createShared<Scene>(); };
	};
	
}