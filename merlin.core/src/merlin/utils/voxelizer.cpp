#include "glpch.h"
#include "voxelizer.h"
#include "merlin/shaders/computeShader.h"

namespace Merlin {

	std::vector<int> Voxelizer::voxelize(Mesh& mesh, float vox_size){
		mesh.computeBoundingBox();

		Vertices vertices = mesh.getVertices();
		Indices indices = mesh.getIndices();

		std::vector<Facet> facets;
	
		int index = 0;
		Facet buf;
		for (const auto& i : indices) {
			index++;
			if(index == 1)
				buf.v0 = glm::vec4(vertices[i].position, 0);
			else if (index == 2)
				buf.v1 = glm::vec4(vertices[i].position, 0);
			else if (index == 3) {
				buf.v1 = glm::vec4(vertices[i].position, 0);
				facets.push_back(buf);
				index = 0;
			}
		}

		BoundingBox bb = mesh.getBoundingBox();
		glm::vec3 bb_size = bb.max - bb.min;

		GLuint voxThread = ceil(bb_size.x / (vox_size)) * ceil(bb_size.y / (vox_size)) * ceil(bb_size.z / (vox_size)); //Total number of bin (thread)
		SSBO_Ptr<int> voxBuffer = SSBO<int>::create("voxel_buffer", voxThread); //full grid
		SSBO_Ptr<Facet> facetBuffer = SSBO<Facet>::create("vertex_buffer", voxThread); //full grid

		ComputeShader voxelize("voxelize", "./assets/common/shaders/voxelize.comp");
		voxelize.attach(*voxBuffer);
		voxelize.attach(*facetBuffer);

		voxelize.setVec3("",bb.min);
		voxelize.setVec3("", bb.max);
		voxelize.setFloat("", vox_size);
		voxelize.setUInt("", facets.size());

		GLuint pWkgSize = 256;
		GLuint pWkgCount = (facets.size() + pWkgSize - 1) / pWkgSize; //Total number of workgroup needed

		voxelize.dispatch(pWkgCount);
		voxelize.barrier();

		voxBuffer->bind();
		return voxBuffer->read();
	}

}