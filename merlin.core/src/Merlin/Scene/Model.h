#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Graphics/Mesh.h"
#include "Merlin/Graphics/Material.h"
#include "Merlin/Scene/Camera.h"
#include "Merlin/Graphics/RenderableObject.h"

#include <map>

namespace Merlin {

    class Model : public RenderableObject {
    public:

        Model(const std::string& name);
        Model(const std::string& name, const Shared<Mesh>& mesh);
        Model(const std::string& name, const std::vector<Shared<Mesh>>& mesh);

        void AddMesh(const Shared<Mesh>& mesh);
        void Draw(const Camera& camera) const;


        void SetShader(const Shared<Shader>& shader);
        void SetMaterial(const Shared<Material>& material);
        void SetShader(const std::string& shader);
        void SetMaterial(const std::string& material);

        inline const std::vector<Shared<Mesh>>& meshes() const { return m_meshes; }

        static Shared<Model> Create(std::string name);
        static Shared<Model> Create(const std::string& name, const Shared<Mesh>& mesh);
        static Shared<Model> Create(const std::string& name, const std::vector<Shared<Mesh>>& mesh);



    protected:
        std::vector<Shared<Mesh>> m_meshes;
        //TODO : Group Meshes by Shaders and Material to optimize draw call
    };

    typedef Shared<Model> Model_Ptr;

}