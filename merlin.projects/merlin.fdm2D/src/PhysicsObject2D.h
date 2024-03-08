#pragma once
#include "Merlin.h"
#include <vector>
#include <limits>

struct AABB2D {
	glm::vec2 min;
	glm::vec2 max;
};


class PhysicsObject2D {
public :
	PhysicsObject2D(std::string name, const std::vector<glm::vec2>& surface);
	bool isPointInside(const glm::vec2& point);
	Mesh_Ptr m_mesh = nullptr;
	void drawSDF();

private:

	void generateAABB();
	void generateSDF();
	void generateSSBO();
	void generateMesh();

	AABB2D m_aabb;
	std::vector<glm::vec2> m_surface;
	SSBO_Ptr<glm::vec2> m_surface_ssbo = nullptr;
	

	Texture_Ptr m_sdf = nullptr;

	


	Shader_Ptr m_SDF_shader;
	unsigned int quadVAO;
	glm::vec2 m_position = glm::vec2(0);
	float m_rotation = 0;



	std::string m_name;
};

