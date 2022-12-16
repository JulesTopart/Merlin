#pragma once
#include "../Renderer/Mesh.h"

using namespace Merlin::Renderer;

namespace Merlin::Utils::MeshLoader{

	typedef VertexBufferLayout DataLayout;

	enum SupportedFileType {
		OBJ,
		STL,
		BINARY,
		UNKNOWN
	};

	std::shared_ptr<Renderer::Mesh> ImportMesh(std::string name, std::string filepath, DataLayout dl = DataLayout());
}