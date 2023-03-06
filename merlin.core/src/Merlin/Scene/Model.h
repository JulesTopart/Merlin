#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Renderer/Mesh.h"
#include "Merlin/Scene/Material.h"
#include "Merlin/Scene/Camera.h"


namespace Merlin::Renderer {

    class Model {
    public:
        Model(const Shared<Mesh>& mesh);
        Model(const Shared<Mesh>& mesh, Shared<Material> material);

        void SetMaterial(Shared<Material> material);

        const Mesh& mesh() const;
        const Material& material() const;
        void Draw(const Shader& shader, glm::mat4 view) const;

    private:
        Shared<Mesh> _mesh;
        Shared<Material> _material;

        static Shared<Material> defaultMaterial;
    };


}