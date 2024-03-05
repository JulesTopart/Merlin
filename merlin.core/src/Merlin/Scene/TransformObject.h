#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Graphics/Mesh.h"
#include "Merlin/Graphics/Material.h"
#include "Merlin/Scene/Camera.h"
#include "Merlin/Graphics/RenderableObject.h"


#include <map>

namespace Merlin {

    class TransformObject : public RenderableObject {
    public:

        TransformObject(const std::string& name);
        static Shared<TransformObject> create(std::string name);

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