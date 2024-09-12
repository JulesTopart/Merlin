#include "glpch.h"
#include "voxelizer.h"
#include "merlin/memory/bindingPointManager.h"
#include "merlin/shaders/computeShader.h"

namespace Merlin {

	std::vector<int> Voxelizer::voxelize(Mesh& mesh, float vox_size) {
		return voxelize(mesh, vox_size, 0);
	}

	std::vector<int> Voxelizer::voxelizeSurface(Mesh& mesh, float vox_size, float thickness) {
		return voxelize(mesh, vox_size, thickness);
	}

	std::vector<glm::vec3> Voxelizer::getVoxelposition(const std::vector<int>& voxels, const BoundingBox& aabb, float spacing){
		std::vector<glm::vec3> voxel_positions;

		glm::vec3 bb_size = aabb.max - aabb.min;
		if (bb_size.x == 0) bb_size.x += spacing;
		if (bb_size.y == 0) bb_size.y += spacing;
		if (bb_size.z == 0) bb_size.z += spacing;
		int gridSizeX = ceil(bb_size.x / spacing);
		int gridSizeY = ceil(bb_size.y / spacing);
		int gridSizeZ = ceil(bb_size.z / spacing);



		for (int i = 0; i < gridSizeX * gridSizeY * gridSizeZ; i++) {
			if (voxels[i] != 0) {

				int index = i;
				int vz = index / (gridSizeX * gridSizeY);
				index -= (vz * gridSizeX * gridSizeY);
				int vy = index / gridSizeX;
				int vx = index % gridSizeX;

				float x = aabb.min.x + (vx + 0.5) * spacing;
				float y = aabb.min.y + (vy + 0.5) * spacing;
				float z = aabb.min.z + (vz + 0.5) * spacing;

				voxel_positions.push_back(glm::vec4(x, y, z, 0.0));
			}
		}
		
		return voxel_positions;
	}

	std::vector<int> Voxelizer::voxelize(Mesh& mesh, float vox_size, float thickness) {

		Vertices vertices = mesh.getVertices();
		Indices indices = mesh.getIndices();

		std::vector<Facet> facets;

		int index = 0;
		Facet buf;

		if(indices.size() > 0) {
			for (const auto& i : indices) {
				index++;
				if (index == 1)
					buf.v0 = glm::vec4(vertices[i].position, 1);
				else if (index == 2)
					buf.v1 = glm::vec4(vertices[i].position, 1);
				else if (index == 3) {
					buf.v2 = glm::vec4(vertices[i].position, 1);
					facets.push_back(buf);
					index = 0;
				}
			}
		}else{
			for (int i = 0; i < vertices.size(); i++) {
				index++;
				if (index == 1)
					buf.v0 = glm::vec4(vertices[i].position, 1);
				else if (index == 2)
					buf.v1 = glm::vec4(vertices[i].position, 1);
				else if (index == 3) {
					buf.v2 = glm::vec4(vertices[i].position, 1);
					facets.push_back(buf);
					index = 0;
				}
			}
		}

		mesh.computeBoundingBox();
		BoundingBox bb = mesh.getBoundingBox();
		glm::vec3 bb_size = bb.max - bb.min;
		if(bb_size.x == 0) bb_size.x += vox_size;
		if(bb_size.y == 0) bb_size.y += vox_size;
		if(bb_size.z == 0) bb_size.z += vox_size;

		GLuint voxThread = ceil(bb_size.x / vox_size) * ceil(bb_size.y / vox_size) * ceil(bb_size.z / vox_size); //Total number of bin (thread)
		SSBO_Ptr<GLint> voxBuffer = SSBO<GLint>::create("voxel_buffer", voxThread); //full grid
		SSBO_Ptr<Facet> facetBuffer = SSBO<Facet>::create("vertex_buffer", facets.size(), facets.data()); //full grid

		if (!m_voxelize) m_voxelize = ComputeShader::create("voxelize", "./assets/common/shaders/voxelize.comp");

		m_voxelize->use();
		m_voxelize->attach(*voxBuffer);
		m_voxelize->attach(*facetBuffer);

		m_voxelize->setVec4("aabbMin", glm::vec4(bb.min, 1));
		m_voxelize->setVec4("aabbMax", glm::vec4(bb.max, 1));
		m_voxelize->setMat4("modelMatrix", mesh.globalTransform());
		m_voxelize->setFloat("voxelSize", vox_size);
		m_voxelize->setUInt("facetCount", facets.size());
		m_voxelize->setUInt("voxelCount", voxThread);
		m_voxelize->setFloat("surface_thickness", thickness);

		GLuint pWkgSize = 64;
		GLuint pWkgCount = (voxThread + pWkgSize - 1) / pWkgSize; //Total number of workgroup needed

		m_voxelize->dispatch(pWkgCount);
		m_voxelize->barrier();

		facetBuffer->releaseBindingPoint();
		voxBuffer->releaseBindingPoint();

		return voxBuffer->read();
	}



}