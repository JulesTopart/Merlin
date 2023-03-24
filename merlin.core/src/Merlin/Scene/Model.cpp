#include "glpch.h"
#include "Model.h"

namespace Merlin::Graphics {

    Shared<Material> Model::defaultMaterial = nullptr;

    Shared<Model> Model::Create(Shared<Mesh> mesh) {
        return std::make_shared<Model>(mesh);
    }

    Shared<Model> Model::Create(Shared<Mesh> mesh, Shared<Material> material) {
        return std::make_shared<Model>(mesh, material);
    }

    Model::Model(Shared<Mesh> mesh)
        : _mesh(mesh) {
        _material = defaultMaterial;
        _transform = glm::mat4(1.0f);
    }

    Model::Model(Shared<Mesh> mesh, Shared<Material> mat)
        : _mesh(mesh) {
        _material = mat;
        _transform = glm::mat4(1.0f);
    }

    void Model::SetMaterial(Shared<Material> material) {
        _material = material;
    }

    void Model::Translate(glm::vec3 v) {
       _transform = glm::translate(_transform, v);
    }
    
    void Model::Rotate(glm::vec3 v) {
        _transform = glm::rotate(_transform, v.x, glm::vec3(1, 0, 0));
        _transform = glm::rotate(_transform, v.y, glm::vec3(0, 1, 0));
        _transform = glm::rotate(_transform, v.z, glm::vec3(0, 0, 1));
    }

    void Model::Rotate(float angle, glm::vec3 v) {
        _transform = glm::rotate(_transform, angle, v);
    }

    void Model::SetTransform(glm::mat4 t) {
        _transform = t;
    }


    void Model::Draw(const Camera& camera) const {
        Shared<Shader> sh = _material->GetShader();
            
        sh->Use();
        sh->SetVec3("viewPos", camera.GetPosition()); //Sync camera with GPU
        sh->SetMat4("view", camera.GetViewProjectionMatrix()); //Sync camera with GPU
        sh->SetMat4("model", _transform); //Sync camera with GPU

        sh->SetVec3("ambient", _material->ambient());
        sh->SetVec3("diffuse", _material->diffuse());
        sh->SetVec3("specular", _material->specular());
        sh->SetFloat("shininess", _material->shininess());
        
        //manage camera,transform, textures

        unsigned int numDiffuse = 0;
        unsigned int numSpecular = 0;

        for (size_t i = 0; i < _material->GetTextureCount(); i++) {
            std::string num;
            Shared<Texture> tex = _material->GetTexture(i);
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
            tex->SyncTextureUnit(*_material->GetShader(), (tex->typeToString() + num));
            tex->Bind();
        }
        


        _mesh->Draw();
    }

}