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
        Shared<T> get(const std::string& name);
        bool exist(const std::string& name);

        inline unsigned int size() { return resources.size(); };

    protected:
        std::unordered_map<std::string, std::shared_ptr<T>> resources;
    };

    template <typename T>
    void RessourceManager<T>::add(const std::string& name, std::shared_ptr<T> resource) {
        if (RessourceManager<T>::exist(name)) Console::warn("RessourceManager") << name << " already exist, it has be overrided" << Console::endl;
        resources[name] = resource;

    }

    template <typename T>
    std::shared_ptr<T> RessourceManager<T>::get(const std::string& name) {
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
        SINGLETON(ShaderLibrary)
    public:
        inline void add(Shared<Shader> shader){ RessourceManager::add(shader->name(), shader); };

    private: 
        ShaderLibrary();
        void LoadDefaultShaders();
        
    };


    class MaterialLibrary : public RessourceManager<MaterialBase> {
        SINGLETON(MaterialLibrary)
    public:
        inline void add(Shared<MaterialBase> mat) { RessourceManager::add(mat->name(), mat); };

    private:
        MaterialLibrary();
        void LoadDefaultMaterials();
    };
}