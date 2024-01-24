#pragma once

#include "Merlin/Core/Core.h"
#include "Merlin/Graphics/Material.h"

#include <unordered_map>

namespace Merlin::Graphics {

    template <typename T>
    class RessourceManager {
    public:
        RessourceManager() = default;

        void Add(const std::string& name, std::shared_ptr<T> resource);
        const T& Get(const std::string& name);
        std::shared_ptr<T> Share(const std::string& name);
        bool Exist(const std::string& name);

        inline unsigned int size() { return resources.size(); };

    protected:
        std::unordered_map<std::string, std::shared_ptr<T>> resources;
    };


    class ShaderLibrary : public RessourceManager<Shader> {
    public:
        ShaderLibrary();
        inline void Add(Shared<Shader> shader){ RessourceManager::Add(shader->name(), shader); };

    private: 
        void LoadDefaultShaders();
    };

    class MaterialLibrary : public RessourceManager<Material> {
    public:
        MaterialLibrary();
        inline void Add(Shared<Material> mat) { RessourceManager::Add(mat->name(), mat); };

    private:
        void LoadDefaultMaterials();
    };
}