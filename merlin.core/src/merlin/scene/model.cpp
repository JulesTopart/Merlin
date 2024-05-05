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

	void Model::setMaterial(Shared<MaterialBase> material) {
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
				Shared<Shader> shader = mesh->getShader();
				Shared<MaterialBase> mat = mesh->getMaterial();

                shader->use();
                mat->attach(*shader);

				mesh->draw();
            }
        }
    }



}