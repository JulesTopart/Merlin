#pragma once
#include "merlin/graphics/mesh.h"
#include "merlin/memory/shaderStorageBuffer.h"

namespace Merlin {
	class Voxelizer {
	public:
		//static SSBO_Ptr<glm::vec4> voxelize_sparse(Mesh& mesh, float vox_size);
		static std::vector<int> voxelize(Mesh& mesh, float vox_size);

	private :
		struct Facet {
			alignas(16) glm::vec4 v0;
			alignas(16) glm::vec4 v1;
			alignas(16) glm::vec4 v2;
			alignas(16) glm::vec4 dummy;
		};

	};

}

