#pragma once

#include "Merlin/Core/Core.h"
#include "Merlin/Graphics/Material.h"

#include <unordered_map>

template <typename T>
class ResourceManager {
public:
    static ResourceManager& Instance() {
        static ResourceManager instance;
        return instance;
    }

    void Add(const std::string& name, std::shared_ptr<T> resource) {
        resources[name] = resource;
    }

    std::shared_ptr<T> Get(const std::string& name) const {
        auto it = resources.find(name);
        if (it != resources.end()) {
            return it->second;
        }
        // Handle the case where the resource is not found
        return nullptr;
    }

    // Delete copy constructor and assignment operator
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

private:
    ResourceManager() = default;
    std::unordered_map<std::string, std::shared_ptr<T>> resources;
};


// ShaderLibrary class derived from ResourceManager
class ShaderLibrary : public ResourceManager<Shader> {

};

class ShaderLibrary {
public:
    ShaderLibrary();

    void Add(Shared<Shader> shader);
    void Load(const std::string& filepath);
    const Shader& Get(const std::string& name);
    Shared<Shader> Share(const std::string& name);

    bool Exists(const std::string& name);
private:
    std::unordered_map<std::string, Shared<Shader>> _shaders;
};

// MaterialLibrary class derived from ResourceManager
class MaterialLibrary : public ResourceManager<Material> {
    // Specific functionalities for MaterialLibrary
    // ...
};