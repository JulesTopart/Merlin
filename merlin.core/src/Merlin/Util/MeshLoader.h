#pragma once
#include "../Renderer/Mesh.h"

namespace Merlin::Utils::MeshLoader{

	std::shared_ptr<Renderer::Mesh> LoadObject(std::string filepath, std::string name = "3D Object");
		 



}