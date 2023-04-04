#include "glpch.h"
#include "Model.h"

namespace Merlin::Graphics {


    Model::Model(const std::string& name) : _name(name){
        _transform = glm::mat4(1.0f);
    }

    Model::Model(const std::string& name, const Shared<Mesh>& mesh) {
        _meshes.push_back(mesh);
        _transform = glm::mat4(1.0f);
    }

    Model::Model(const std::string& name, const std::vector<Shared<Mesh>>& meshes, const std::vector<glm::mat4> transforms) {
        _meshes = meshes;
        _meshTransform = transforms;
        _transform = glm::mat4(1.0f);
    }


    void AddMesh(const Shared<Mesh>& mesh, glm::mat4 meshTransform) {

    }






    void Model::Draw(const Renderer& renderer,  const Camera& camera, const Shader& shader) const {
        shader.Use();
        shader.SetVec3("viewPos", camera.GetPosition()); //Sync camera with GPU
        shader.SetMat4("view", camera.GetViewProjectionMatrix()); //Sync camera with GPU
        for (int index = 0; index < _meshes.size(); index++) {
            renderer.Get
            shader.SetMat4("model", _transform); //Sync model matrix with GPU
            shader.SetVec3("ambient", material.ambient());
            shader.SetVec3("diffuse", material.diffuse());
            shader.SetVec3("specular", material.specular());
            shader.SetFloat("shininess", material.shininess());
        }
        



        //manage camera,transform, textures

        unsigned int numDiffuse = 0;
        unsigned int numSpecular = 0;

        for(size_t i = 0; i < material.GetTextureCount(); i++) {
            std::string num;
            Shared<Texture> tex = material.GetTexture(i);
            if (tex->type() == TextureType::DIFFUSE) {
                num = std::to_string(numDiffuse++);
            }
            else if (tex->type() == TextureType::SPECULAR) {
                num = std::to_string(numSpecular++);
            }
            else {
                Console::error() << "Invalid texture type" << Console::endl;
            }
            tex->SetUnit(i);
            tex->SyncTextureUnit(shader, (tex->typeToString() + num));
            tex->Bind();
        }



        _meshes->Draw();
    }



}