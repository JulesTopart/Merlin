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

        inline Shared<Mesh> Model::GetMesh() const { return _mesh; }

        inline glm::vec3& position() {
            glm::vec3 scale;
            glm::quat rotation;
            glm::vec3 translation;
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(_transform, scale, rotation, translation, skew, perspective);
            return translation;
        }

        inline glm::quat& rotation() {
            glm::vec3 scale;
            glm::quat rotation;
            glm::vec3 translation;
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(_transform, scale, rotation, translation, skew, perspective);
            return rotation;
        }

        inline glm::mat4& transform() { return _transform; }
        inline const Mesh& Model::mesh() const { return *_mesh; }
        inline const Material& Model::material() const { return *_material; }

    private:
        glm::mat4 _transform;
        Shared<Mesh> _mesh;
        Shared<Material> _material;
        
        static Shared<Material> defaultMaterial;
    };


}