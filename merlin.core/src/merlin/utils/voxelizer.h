#pragma once
#include "merlin/graphics/mesh.h"
#include "merlin/memory/ssbo.h"
#include "merlin/shaders/computeShader.h"

namespace Merlin {
	class Voxelizer {
	public:
		//static SSBO_Ptr<glm::vec4> voxelize_sparse(Mesh& mesh, float vox_size);
		static std::vector<int> voxelize(Mesh& mesh, float vox_size);
		static std::vector<int> voxelizeSurface(Mesh& mesh, float vox_size, float thickness);

		static std::vector<glm::vec3> getVoxelposition(const std::vector<int>& voxels, const BoundingBox&, float spacing);

	private :
		static std::vector<int> voxelize(Mesh& mesh, float vox_size, float thickness);
		inline static ComputeShader_Ptr m_voxelize = nullptr;


		struct Facet {
			alignas(16) glm::vec4 v0;
			alignas(16) glm::vec4 v1;
			alignas(16) glm::vec4 v2;
			alignas(16) glm::vec4 dummy;
		};

	};

}

