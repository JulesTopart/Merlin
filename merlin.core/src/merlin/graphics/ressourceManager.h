#pragma once

#include "merlin/core/core.h"
#include "merlin/graphics/material.h"

#include <unordered_map>

namespace Merlin {

    template <typename T>
    class RessourceManager {
    public:
        RessourceManager() = default;

        void add(const std::string& name, std::shared_ptr<T> resource);
        const T& get(const std::string& name);
        std::shared_ptr<T> share(const std::string& name);
        bool exist(const std::string& name);

        inline unsigned int size() { return resources.size(); };

    protected:
        std::unordered_map<std::string, std::shared_ptr<T>> resources;

        SINGLETON(RessourceManager)
    };

    template <typename T>
    void RessourceManager<T>::add(const std::string& name, std::shared_ptr<T> resource) {
        if (RessourceManager<T>::exist(name)) Console::warn("RessourceManager") << name << " already exist, it has be overrided" << Console::endl;
        resources[name] = resource;

    }

    template <typename T>
    const T& RessourceManager<T>::get(const std::string& name) {
        auto it = resources.find(name);
        if (it != resources.end()) {
            return *it->second;
        }
        Console::error("Ressource Manager") << name << " not found in library" << Console::endl;
        // Handle the case where the resource is not found
        throw;
    }

    template <typename T>
    std::shared_ptr<T> RessourceManager<T>::share(const std::string& name) {
        auto it = resources.find(name);
        if (it != resources.end()) {
            return it->second;
        }
        // Handle the case where the resource is not found
        return nullptr;
    }

    template <typename T>
    bool RessourceManager<T>::exist(const std::string& name) {
        return resources.find(name) != resources.end();
    }




    class ShaderLibrary : public RessourceManager<Shader> {
    public:
        ShaderLibrary();
        inline void add(Shared<Shader> shader){ RessourceManager::add(shader->name(), shader); };

    private: 
        void LoadDefaultShaders();

        SINGLETON(ShaderLibrary)
    };

    class MaterialLibrary : public RessourceManager<Material> {
    public:
        MaterialLibrary();
        inline void add(Shared<Material> mat) { RessourceManager::add(mat->name(), mat); };

    private:
        void LoadDefaultMaterials();

        SINGLETON(MaterialLibrary)
    };
}