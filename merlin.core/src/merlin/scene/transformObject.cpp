#include "glpch.h"
#include "transformObject.h"
#include "merlin/utils/primitives.h"

namespace Merlin {

    TransformObject::TransformObject(const std::string& name, float size) : TransformObject::RenderableObject(name) {
        x_axis = Primitives::createCylinder(size/40, size, 10);
        y_axis = Primitives::createCylinder(size/40, size, 10);
        z_axis = Primitives::createCylinder(size/40, size, 10);

        /*
        x_axis->calculateNormals();
        y_axis->calculateNormals();
        z_axis->calculateNormals();
        */

        Shared<PhongMaterial> xMaterial = createShared<PhongMaterial>("xMaterial");
        Shared<PhongMaterial> yMaterial = createShared<PhongMaterial>("yMaterial");
        Shared<PhongMaterial> zMaterial = createShared<PhongMaterial>("zMaterial");

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

    Shared<TransformObject> TransformObject::create(std::string name, float size) {
        return std::make_shared<TransformObject>(name, size);
    }



}