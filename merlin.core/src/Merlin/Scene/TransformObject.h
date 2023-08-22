#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Graphics/Mesh.h"
#include "Merlin/Scene/Material.h"
#include "Merlin/Scene/Camera.h"
#include "Merlin/Graphics/RenderableObject.h"

#include <map>

namespace Merlin::Graphics {

    class TransformObject : public RenderableObject {
    public:

        TransformObject(const std::string& name);
        static Shared<TransformObject> Create(std::string name);
    };


}