#include "glpch.h"
#include "Model.h"

namespace Merlin::Graphics {


    Model::Model(const std::string& name) : RenderableObject(name){
        m_transform = glm::mat4(1.0f);
    }

    Model::Model(const std::string& name, const Shared<Mesh>& mesh) : RenderableObject(name) {
        m_meshes.push_back(mesh);
        m_transform = glm::mat4(1.0f);
    }

    Model::Model(const std::string& name, const std::vector<Shared<Mesh>>& meshes) : RenderableObject(name) {
        m_meshes = meshes;
        m_transform = glm::mat4(1.0f);
    }

    Shared<Model> Model::Create(std::string name) {
        return CreateShared<Model>(name);
    }

    Shared<Model> Model::Create(const std::string& name, const Shared<Mesh>& mesh) {
        return CreateShared<Model>(name, mesh);
    }

    Shared<Model> Model::Create(const std::string& name, const std::vector<Shared<Mesh>>& meshes) {
        return CreateShared<Model>(name, meshes);
    }


    void Model::AddMesh(const Shared<Mesh>& mesh) {
        m_meshes.push_back(mesh);
    }

	void Model::SetShader(const Shared<Shader>& shader) {
		for (auto& mesh : m_meshes) {
			mesh->SetShader(shader);
		}
	}

	void Model::SetMaterial(const Shared<Material>& material) {
		for (auto& mesh : m_meshes) {
			mesh->SetMaterial(material);
		}
	}

	void Model::SetShader(const std::string& shader) {
		for (auto& mesh : m_meshes) {
			mesh->SetShader(shader);
		}
	}

	void Model::SetMaterial(const std::string& material) {
		for (auto& mesh : m_meshes) {
			mesh->SetMaterial(material);
		}
	}


    void Model::Draw(const Camera& camera) const {
        for (const auto& mesh : m_meshes) {
            if (mesh->HasMaterial() && mesh->HasShader()) {
				const Shader* shader = &mesh->GetShader();
				const Material& mat = mesh->GetMaterial();

				shader->SetVec3("ambient", mat.ambient());
				shader->SetVec3("diffuse", mat.diffuse());
				shader->SetVec3("specular", mat.specular());
				shader->SetFloat("shininess", mat.shininess());

				//manage camera,transform, textures

				Texture* tex;
				tex = &mat.GetTexture(TextureType::COLOR);
				tex->SetUnit(0);
				tex->SyncTextureUnit(*shader, (tex->typeToString()));
				tex->Bind();

				tex = &mat.GetTexture(TextureType::ROUGHNESS);
				tex->SetUnit(1);
				tex->SyncTextureUnit(*shader, (tex->typeToString()));
				tex->Bind();

				mesh->Draw();
            }
        }
    }



}