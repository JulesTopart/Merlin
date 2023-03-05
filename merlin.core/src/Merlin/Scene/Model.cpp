#include "glpch.h"
#include "Model.h"

using namespace Merlin::Renderer;

namespace Merlin::Scene {

    Shared<Material> Model::defaultMaterial = nullptr;

    Model::Model(const Shared<Mesh>& mesh)
        : _mesh(mesh) {
        if (defaultMaterial == nullptr) defaultMaterial = CreateShared<Material>(glm::vec3(0.5), glm::vec3(0.8), glm::vec3(1), 0.3); //Default material color
        _material = defaultMaterial;
    }

    Model::Model(const Shared<Mesh>& mesh, const Shared<Material> mat)
        : _mesh(mesh) {
        if (defaultMaterial == nullptr) defaultMaterial = CreateShared<Material>(glm::vec3(0.5), glm::vec3(0.8), glm::vec3(1), 0.3); //Default material color
        _material = mat;
    }

    void Model::SetMaterial(Shared<Material> material) {
        _material = material;
    }

    const Mesh& Model::mesh() const { return *_mesh; }
    const Material& Model::material() const { return *_material; }

    void Model::Draw(const Shader& shader, glm::mat4 view) const {
        shader.Use();
        
        shader.SetVec3("material.ambient", _material->ambient());
        shader.SetVec3("material.diffuse", _material->diffuse());
        shader.SetVec3("material.specular", _material->specular());
        shader.SetFloat("material.shininess", _material->shininess());
        
        _mesh->Draw(shader, view);
    }

}