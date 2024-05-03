#include "glpch.h"
#include "model.h"

namespace Merlin {


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

    Shared<Model> Model::create(std::string name) {
        return createShared<Model>(name);
    }

    Shared<Model> Model::create(const std::string& name, const Shared<Mesh>& mesh) {
        return createShared<Model>(name, mesh);
    }

    Shared<Model> Model::create(const std::string& name, const std::vector<Shared<Mesh>>& meshes) {
        return createShared<Model>(name, meshes);
    }


    void Model::addMesh(const Shared<Mesh>& mesh) {
        m_meshes.push_back(mesh);
    }

	void Model::setShader(const Shared<Shader>& shader) {
		for (auto& mesh : m_meshes) {
			mesh->setShader(shader);
		}
	}

	void Model::setMaterial(const Shared<Material>& material) {
		for (auto& mesh : m_meshes) {
			mesh->setMaterial(material);
		}
	}

	void Model::setShader(const std::string& shader) {
		for (auto& mesh : m_meshes) {
			mesh->setShader(shader);
		}
	}

	void Model::setMaterial(const std::string& material) {
		for (auto& mesh : m_meshes) {
			mesh->setMaterial(material);
		}
	}


    void Model::draw(const Camera& camera) const {
        for (const auto& mesh : m_meshes) {
            if (mesh->hasMaterial() && mesh->hasShader()) {
				const Shader* shader = &mesh->getShader();
				const Material& mat = mesh->getMaterial();

				shader->setVec3("ambient", mat.ambient());
				shader->setVec3("diffuse", mat.diffuse());
				shader->setVec3("specular", mat.specular());
				shader->setFloat("shininess", mat.shininess());

				//manage camera,transform, textures

				Texture2D* tex;
				tex = &mat.getTexture(TextureType::COLOR);
				tex->setUnit(0);
				tex->syncTextureUnit(*shader, (tex->typeToString()));
				tex->bind();

				tex = &mat.getTexture(TextureType::ROUGHNESS);
				tex->setUnit(1);
				tex->syncTextureUnit(*shader, (tex->typeToString()));
				tex->bind();

				mesh->draw();
            }
        }
    }



}