#include "glpch.h"
#include "TransformObject.h"
#include "Merlin/Util/Primitives.h"

namespace Merlin::Graphics {
    using namespace Merlin::Utils;

    TransformObject::TransformObject(const std::string& name) : TransformObject::RenderableObject(name) {
        x_axis = Primitives::CreateCylinder(0.1, 5, 1000);
        y_axis = Primitives::CreateCylinder(0.1, 5, 1000);
        z_axis = Primitives::CreateCylinder(0.1, 5, 10);

        /*
        x_axis->CalculateNormals();
        y_axis->CalculateNormals();
        z_axis->CalculateNormals();
        */

        Material_Ptr xMaterial = CreateShared<Material>("xMaterial");
        Material_Ptr yMaterial = CreateShared<Material>("yMaterial");
        Material_Ptr zMaterial = CreateShared<Material>("zMaterial");

        zMaterial->SetAmbient(glm::vec3(0.0, 0.00, 0.7));
        zMaterial->SetDiffuse(glm::vec3(0.3, 0.3, 0.5));
        zMaterial->SetSpecular(glm::vec3(0.04, 0.04, 0.7));
        zMaterial->SetShininess(0.078125);

        yMaterial->SetAmbient(glm::vec3(0.0, 0.7, 0.0));
        yMaterial->SetDiffuse(glm::vec3(0.4, 0.5, 0.4));
        yMaterial->SetSpecular(glm::vec3(0.04, 0.7, 0.04));
        yMaterial->SetShininess(0.078125);

        xMaterial->SetAmbient(glm::vec3(0.7, 0.0, 0.0));
        xMaterial->SetDiffuse(glm::vec3(0.5, 0.4, 0.4));
        xMaterial->SetSpecular(glm::vec3(0.7, 0.04, 0.04));
        xMaterial->SetShininess(0.078125);

        x_axis->SetMaterial(xMaterial);
        y_axis->SetMaterial(yMaterial);
        z_axis->SetMaterial(zMaterial);

        /*
        x_axis->SetShader("model");
        y_axis->SetShader("model");
        z_axis->SetShader("model");
        */
        y_axis->Rotate(glm::vec3(-3.1415926/2.0,0,0));
        x_axis->Rotate(glm::vec3(0,3.1415926/2.0,0));

    }

    Shared<TransformObject> TransformObject::Create(std::string name) {
        return std::make_shared<TransformObject>(name);
    }



}