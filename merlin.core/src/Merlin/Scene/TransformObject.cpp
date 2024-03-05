#include "glpch.h"
#include "TransformObject.h"
#include "Merlin/Util/Primitives.h"

namespace Merlin {

    TransformObject::TransformObject(const std::string& name) : TransformObject::RenderableObject(name) {
        x_axis = Primitives::createCylinder(0.1, 5, 1000);
        y_axis = Primitives::createCylinder(0.1, 5, 1000);
        z_axis = Primitives::createCylinder(0.1, 5, 10);

        /*
        x_axis->calculateNormals();
        y_axis->calculateNormals();
        z_axis->calculateNormals();
        */

        Material_Ptr xMaterial = createShared<Material>("xMaterial");
        Material_Ptr yMaterial = createShared<Material>("yMaterial");
        Material_Ptr zMaterial = createShared<Material>("zMaterial");

        zMaterial->setAmbient(glm::vec3(0.0, 0.00, 0.7));
        zMaterial->setDiffuse(glm::vec3(0.3, 0.3, 0.5));
        zMaterial->setSpecular(glm::vec3(0.04, 0.04, 0.7));
        zMaterial->setShininess(0.078125);

        yMaterial->setAmbient(glm::vec3(0.0, 0.7, 0.0));
        yMaterial->setDiffuse(glm::vec3(0.4, 0.5, 0.4));
        yMaterial->setSpecular(glm::vec3(0.04, 0.7, 0.04));
        yMaterial->setShininess(0.078125);

        xMaterial->setAmbient(glm::vec3(0.7, 0.0, 0.0));
        xMaterial->setDiffuse(glm::vec3(0.5, 0.4, 0.4));
        xMaterial->setSpecular(glm::vec3(0.7, 0.04, 0.04));
        xMaterial->setShininess(0.078125);

        x_axis->setMaterial(xMaterial);
        y_axis->setMaterial(yMaterial);
        z_axis->setMaterial(zMaterial);

        /*
        x_axis->setShader("model");
        y_axis->setShader("model");
        z_axis->setShader("model");
        */
        y_axis->rotate(glm::vec3(-3.1415926/2.0,0,0));
        x_axis->rotate(glm::vec3(0,3.1415926/2.0,0));

    }

    Shared<TransformObject> TransformObject::create(std::string name) {
        return std::make_shared<TransformObject>(name);
    }



}