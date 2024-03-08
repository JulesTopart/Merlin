#include "PhysicsObject2D.h"

PhysicsObject2D::PhysicsObject2D(std::string name, const std::vector<glm::vec2>& surface) : m_name(name), m_surface(surface) {
	generateMesh();
	generateSSBO();
	generateSDF();
	generateAABB();

	m_SDF_shader = Shader::create("SDF_shader","./assets/shaders/sprite.vert", "./assets/shaders/sprite.frag");

	// configure VAO/VBO
	
	float vertices[] = {
		// pos      // tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	unsigned int VBO;
	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->quadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}


bool PhysicsObject2D::isPointInside(const glm::vec2& point) {
	bool inside = false;
	for (size_t i = 0, j = m_surface.size() - 1; i < m_surface.size(); j = i++) {
		// Check if point is inside the edge from polygon[i] to polygon[j]
		if (((m_surface[i].y > point.y) != (m_surface[j].y > point.y)) &&
			(point.x < (m_surface[j].x - m_surface[i].x) * (point.y - m_surface[i].y) / (m_surface[j].y - m_surface[i].y) + m_surface[i].x)) {
			inside = !inside;
		}
	}
	return inside;
}

void PhysicsObject2D::drawSDF() {
	m_SDF_shader->use();
	// prepare transformations
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(m_position, 0.0f));
	glm::vec2 size = m_aabb.max - m_aabb.min;
	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
	model = glm::rotate(model, glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

	model = glm::scale(model, glm::vec3(size, 1.0f));

	m_SDF_shader->setMat4("model", model);
	m_SDF_shader->setVec3("spriteColor", glm::vec3(1,1,1));

	glActiveTexture(GL_TEXTURE0);
	m_sdf->bind();
	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void PhysicsObject2D::generateAABB() {
	if (m_surface.empty()) {
		return; // Return if the polygon is empty
	}

	float minX = std::numeric_limits<float>::max();
	float minY = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest();
	float maxY = std::numeric_limits<float>::lowest();

	for (const auto& point : m_surface) {
		minX = std::min(minX, point.x);
		minY = std::min(minY, point.y);
		maxX = std::max(maxX, point.x);
		maxY = std::max(maxY, point.y);
	}

	m_aabb = { glm::vec2(minX, minY), glm::vec2(maxX, maxY) };
}


void PhysicsObject2D::generateMesh() {
	if (m_surface.empty()) {
		m_mesh = Mesh::create("nozzle");
	}
	Vertices vertices;
	Indices indices;

	for (glm::vec2& v : m_surface) {
		indices.push_back(vertices.size());
		vertices.push_back({ glm::vec3(v.x, v.y, 0), glm::vec3(0), glm::vec3(0.75164, 0.60648, 0.22648) });
	}
	indices.push_back(0);

	m_mesh = Mesh::create("nozzle", vertices, indices);
	m_mesh->setDrawMode(GL_LINE_STRIP);
}

void PhysicsObject2D::generateSDF(){
	if (m_surface.empty())return;
	if (!m_surface_ssbo) return;
	m_sdf = Texture::create(64, 64, GL_R8, GL_RED, TextureType::DATA);

	ComputeShader sdfGen("sdfGen", "./assets/shaders/sdf.comp");
	sdfGen.use();
	m_sdf->bind();
	m_sdf->bindImage();
	sdfGen.attach(*m_surface_ssbo);
	sdfGen.dispatch(64, 64);
}

void PhysicsObject2D::generateSSBO(){
	if (m_surface.empty())return;
	m_surface_ssbo = SSBO<glm::vec2>::create(m_name + "_surface", m_surface.size());
	m_surface_ssbo->setBindingPoint(0);
}