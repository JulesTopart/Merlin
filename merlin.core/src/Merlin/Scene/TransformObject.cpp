#include "glpch.h"
#include "TransformObject.h"

namespace Merlin::Graphics {


    TransformObject::TransformObject(const std::string& name) : TransformObject::RenderableObject(name) {}

    Shared<TransformObject> TransformObject::Create(std::string name) {
        return std::make_shared<TransformObject>(name);
    }



}