#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Graphics/Mesh.h"
#include "Merlin/Scene/Material.h"
#include "Merlin/Scene/Camera.h"

#include <glm/gtx/matrix_decompose.hpp>

namespace Merlin::Graphics {

    class Model {
    public:

        static Shared<Model> Create(Shared<Mesh> mesh);
        static Shared<Model> Create(Shared<Mesh> mesh, Shared<Material> material);

        Model(Shared<Mesh> mesh);
        Model(Shared<Mesh> mesh, Shared<Material> material);

        void SetMaterial(Shared<Material> material);
        void Draw(const Camera& camera) const; //Draw using current

        //Transformation
        void Translate(glm::vec3);
        void Rotate(glm::vec3);
        void Rotate(float angle, glm::vec3 v);
        void SetTransform(glm::mat4 t);

        inline glm::vec3& position();
        inline glm::quat& rotation();
        inline glm::mat4& transform();
        inline const Mesh& mesh() const { return *_mesh; }
        inline const Material& material() const { return *_material; }

    protected:
        glm::mat4 _transform;
        Shared<Mesh> _mesh;
        Shared<Material> _material;
    };


}