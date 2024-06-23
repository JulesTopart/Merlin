#include "glpch.h"
#include "voxelizer.h"
#include "merlin/shaders/computeShader.h"

namespace Merlin {

	std::vector<int> Voxelizer::voxelize(Mesh& mesh, float vox_size){
		
		Vertices vertices = mesh.getVertices();
		Indices indices = mesh.getIndices();

		std::vector<Facet> facets;
	
		int index = 0;
		Facet buf;
		for (const auto& i : indices) {
			index++;
			if(index == 1)
				buf.v0 = glm::vec4(vertices[i].position, 1);
			else if (index == 2)
				buf.v1 = glm::vec4(vertices[i].position, 1);
			else if (index == 3) {
				buf.v2 = glm::vec4(vertices[i].position, 1);
				facets.push_back(buf);
				index = 0;
			}
		}

		mesh.computeBoundingBox();
		BoundingBox bb = mesh.getBoundingBox();
		glm::vec3 bb_size = bb.max - bb.min;

		GLuint voxThread = ceil(bb_size.x / vox_size) * ceil(bb_size.y / vox_size) * ceil(bb_size.z / vox_size); //Total number of bin (thread)
		SSBO_Ptr<GLint> voxBuffer = SSBO<GLint>::create("voxel_buffer", voxThread); //full grid
		SSBO_Ptr<Facet> facetBuffer = SSBO<Facet>::create("vertex_buffer", facets.size(), facets.data()); //full grid

		ComputeShader voxelize("voxelize", "./assets/common/shaders/voxelize.comp");
		voxelize.use();
		voxelize.attach(*voxBuffer);
		voxelize.attach(*facetBuffer);

		voxelize.setVec4("aabbMin", glm::vec4(bb.min, 1));
		voxelize.setVec4("aabbMax", glm::vec4(bb.max, 1));
		voxelize.setMat4("modelMatrix", mesh.globalTransform());
		voxelize.setFloat("voxelSize", vox_size);
		voxelize.setUInt("facetCount", facets.size());
		voxelize.setUInt("voxelCount", voxThread);

		GLuint pWkgSize = 64;
		GLuint pWkgCount = (voxThread + pWkgSize - 1) / pWkgSize; //Total number of workgroup needed

		voxelize.dispatch(pWkgCount);
		voxelize.barrier();
		facetBuffer->releaseBindingPoint();
		voxBuffer->releaseBindingPoint();

		voxBuffer->bind();
		return voxBuffer->read();
	}

}