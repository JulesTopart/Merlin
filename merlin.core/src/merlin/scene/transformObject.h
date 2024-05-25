#pragma once
#include "merlin/core/core.h"
#include "merlin/graphics/mesh.h"
#include "merlin/graphics/material.h"
#include "merlin/scene/camera.h"
#include "merlin/graphics/renderableObject.h"


#include <map>

namespace Merlin {

    class TransformObject : public RenderableObject {
    public:

        TransformObject(const std::string& name, float size = 1);
        static Shared<TransformObject> create(std::string name, float size = 1);

        inline const Mesh_Ptr& getXAxisMesh() const { return x_axis; }
        inline const Mesh_Ptr& getYAxisMesh() const { return y_axis; }
        inline const Mesh_Ptr& getZAxisMesh() const { return z_axis; }

    private :
        Mesh_Ptr x_axis;
        Mesh_Ptr y_axis;
        Mesh_Ptr z_axis;
    };

    typedef Shared<TransformObject> TransformObject_Ptr;

}