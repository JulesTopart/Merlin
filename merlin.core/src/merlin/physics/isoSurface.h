#pragma once
#include "merlin/core/core.h"

#include "merlin/memory/vao.h"
#include "merlin/memory/ssbo.h"
#include "merlin/shaders/shader.h"
#include "merlin/textures/texture.h"
#include "merlin/shaders/computeShader.h"
#include "merlin/core/timestep.h"
#include "merlin/graphics/mesh.h"

#include "glm/gtc/random.hpp"

namespace Merlin {
	//https://github.com/mwalczyk/marching-cubes/tree/master

	class IsoSurface;
	typedef Shared<IsoSurface> IsoSurface_Ptr;

	class IsoSurface : public RenderableObject{
	public:
		IsoSurface(const std::string& name, glm::ivec3 volumeSize);
		IsoSurface(const std::string& name, Texture3D_Ptr volume);
		
		void setVolumeTexture(Texture3D_Ptr volume);
		Texture3D_Ptr getVolumeTexture();
		void compute();
		inline void setIsoLevel(float lvl) { m_isoLevel = lvl; }
		inline Mesh_Ptr mesh() { return m_mesh; }

		static IsoSurface_Ptr create(const std::string& name, glm::ivec3 volumeSize);
		static IsoSurface_Ptr create(const std::string& name, Texture3D_Ptr volume);

	private:
		void allocateBuffers();
		void loadDefaultShaders();

		Mesh_Ptr m_mesh;
		float m_isoLevel = 0.1;
		glm::ivec3 volume_size = glm::ivec3(128, 128, 128);
		size_t max_number_of_vertices = 0;

		inline static ComputeShader_Ptr default_marchingCubes = nullptr;
		inline static Shader_Ptr default_isosurface = nullptr;

		ComputeShader_Ptr marchingCubes = nullptr;
		Shader_Ptr isosurface = nullptr;

		Texture3D_Ptr m_volume;

		ImmutableSSBO_Ptr<glm::vec4> buffer_vertices;
		ImmutableSSBO_Ptr<glm::vec4> buffer_normals;

		inline static ImmutableSSBO_Ptr<GLint> buffer_triangle_table;
		inline static ImmutableSSBO_Ptr<GLint> buffer_configuration_table;
	};


}