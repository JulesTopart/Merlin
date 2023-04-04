#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Graphics/Mesh.h"
#include "Merlin/Scene/Material.h"
#include "Merlin/Scene/Camera.h"
#include "Merlin/Graphics/Renderer.h"


#include <map>

namespace Merlin::Graphics {

    struct ModelMesh {
        std::string _materialName;
        std::string _shaderName;
        glm::mat4 _transform;
        Shared<Mesh> _mesh;
    };

    class Model {
    public:

        Model(const std::string& name);
        Model(const std::string& name, const Shared<Mesh>& mesh);
        Model(const std::string& name, const std::vector<Shared<Mesh>>& mesh, const std::vector<glm::mat4> transforms);

        void AddMesh(const Shared<Mesh>& mesh, glm::mat4 meshTransform = glm::mat4(1));

        void Draw(const Renderer& renderer, const Camera& camera) const;




        inline const std::vector<Shared<Mesh>>& meshes() const { return _meshes; }
        inline const std::vector<std::string>& materialNames() const { return _materialNames; }
        inline const std::vector<std::string>& shaderNames() const { return _shaderNames; }

        static Shared<Model> Create(std::string name);
        static Shared<Model> Create(const std::string& name, const Shared<Mesh>& mesh);
        static Shared<Model> Create(const std::string& name, const std::vector<Shared<Mesh>>& mesh);



    protected:
        std::vector<ModelMesh> meshes;

        //Group Meshes by Shaders and Material to optimize draw call
        std::map<std::string, std::map<std::string, std::vector<Shared<Mesh>>>> groupedMeshes;
    };


}