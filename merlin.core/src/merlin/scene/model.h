#pragma once
#include "merlin/core/core.h"
#include "merlin/graphics/mesh.h"
#include "merlin/graphics/material.h"
#include "merlin/scene/camera.h"
#include "merlin/graphics/renderableObject.h"

#include <map>

namespace Merlin {

    class Model : public RenderableObject {
    public:

        Model(const std::string& name);
        Model(const std::string& name, const Shared<Mesh>& mesh);
        Model(const std::string& name, const std::vector<Shared<Mesh>>& mesh);

        void addMesh(const Shared<Mesh>& mesh);
        void draw(const Camera& camera) const;


        void setShader(const Shared<Shader>& shader);
        void setMaterial(const Shared<Material>& material);
        void setShader(const std::string& shader);
        void setMaterial(const std::string& material);

        inline const std::vector<Shared<Mesh>>& meshes() const { return m_meshes; }

        static Shared<Model> create(std::string name);
        static Shared<Model> create(const std::string& name, const Shared<Mesh>& mesh);
        static Shared<Model> create(const std::string& name, const std::vector<Shared<Mesh>>& mesh);



    protected:
        std::vector<Shared<Mesh>> m_meshes;
        //TODO : Group Meshes by Shaders and Material to optimize draw call
    };

    typedef Shared<Model> Model_Ptr;

}