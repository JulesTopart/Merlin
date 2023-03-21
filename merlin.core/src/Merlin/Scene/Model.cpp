#include "glpch.h"
#include "Model.h"

namespace Merlin::Graphics {

    Shared<Material> Model::defaultMaterial = nullptr;

    Model::Model(const Shared<Mesh>& mesh)
        : _mesh(mesh) {
        /*
        if (defaultMaterial == nullptr) {
            defaultMaterial = CreateShared<Material>();
            defaultMaterial->SetProperty(glm::vec3(0.5), glm::vec3(0.8), glm::vec3(1), 0.3);
            Shared<Shader> defaultShader = CreateShared<Shader>("default");

            std::string vertexShader =
                "#version 330 core \n \
                layout(location = 0) in vec3 _position;\n \
                layout(location = 1) in vec3 _normal;\n \
                out vec3 position;\n \
                out vec3 normal;\n \
                uniform mat4 view;\n \
                uniform mat4 model;\n \
                void main() {\n \
                    position = vec3(model * vec4(_position, 1.0f));\n \
                    normal = _normal;\n \
                    gl_Position = view * vec4(position, 1.0f);\n \
                }";

            std::string fragmentShader =
                "#version 430 core\n \
                layout(location = 0) out vec4 o_Color;\n \
                in vec3 normal;\n \
                void main() {\n \
                    o_Color = vec4(vec3(0.5f) + max(dot(normalize(normal), normalize(vec3(1, 1, -2))), 0.0), 1.0f);\n \
                }";

           
            defaultShader->CompileFromSrc(vertexShader, fragmentShader);

            defaultMaterial->SetShader(defaultShader);
        }
        */
        _material = defaultMaterial;
        _transform = glm::mat4(1.0f);
    }

    Model::Model(const Shared<Mesh>& mesh, const Shared<Material>& mat)
        : _mesh(mesh) {
        if (defaultMaterial == nullptr) {
            defaultMaterial = CreateShared<Material>();
            defaultMaterial->SetProperty(glm::vec3(0.5), glm::vec3(0.8), glm::vec3(1), 0.3);
            Shared<Shader> defaultShader = CreateShared<Shader>("default");

            std::string vertexShader =
                "#version 330 core \
                layout(location = 0) in vec3 _position; \
                layout(location = 1) in vec3 _normal; \
                out vec3 position; \
                out vec3 normal; \
                uniform mat4 view; \
                uniform mat4 model; \
                void main() { \
                    position = vec3(model * vec4(_position, 1.0f)); \
                    normal = _normal; \
                    gl_Position = view * vec4(position, 1.0f); \
                }";

            std::string fragmentShader =
                "#version 430 core \
                layout(location = 0) out vec4 o_Color; \
                in vec3 normal; \
                void main() { \
                    o_Color = vec4(vec3(0.5f) + max(dot(normalize(normal), normalize(vec3(1, 1, -2))), 0.0), 1.0f);\
                }";


            defaultShader->CompileFromSrc(vertexShader, fragmentShader);

            defaultMaterial->SetShader(defaultShader);
        }
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


    void Model::Draw(glm::mat4 view) const {
        Shared<Shader> sh = _material->GetShader();
            
        sh->Use();
        sh->SetMat4("view", view); //Sync camera with GPU
        sh->SetMat4("model", _transform); //Sync camera with GPU

        /*
        _material->GetShader()->SetVec3("material.ambient", _material->ambient());
        _material->GetShader()->SetVec3("material.diffuse", _material->diffuse());
        _material->GetShader()->SetVec3("material.specular", _material->specular());
        _material->GetShader()->SetFloat("material.shininess", _material->shininess());
        
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
        */


        _mesh->Draw();
    }

}