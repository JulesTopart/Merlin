#include "glpch.h"
#include "bindingPointManager.h"

namespace Merlin {

    GLuint BindingPointManager::allocateBindingPoint(BufferTarget bufferType) {
        if (availableBindingPoints[bufferType].empty()) {
            Console::error("BindingPointManager") << "No available binding points for the given buffer type" << Console::endl;
            throw std::runtime_error("No available binding points for the given buffer type");
        }
        GLuint bindingPoint = availableBindingPoints[bufferType].front();
        availableBindingPoints[bufferType].pop();
        usedBindingPoints[bufferType].push_back(bindingPoint);
        Console::trace("BindingPointManager") << bindingPoint << " allocated" << Console::endl;
        return bindingPoint;
    }

    void BindingPointManager::releaseBindingPoint(BufferTarget bufferType, GLuint bindingPoint) {
        if (bindingPoint > 16) return;

        auto& points = usedBindingPoints[bufferType];
        points.erase(std::remove(points.begin(), points.end(), bindingPoint), points.end());
        // Ensure binding points are added back in the correct order
        std::queue<GLuint> tempQueue;
        tempQueue.push(bindingPoint);
        while (!availableBindingPoints[bufferType].empty()) {
            tempQueue.push(availableBindingPoints[bufferType].front());
            availableBindingPoints[bufferType].pop();
        }
        availableBindingPoints[bufferType] = tempQueue;

        Console::trace("BindingPointManager") << bindingPoint << " freed" << Console::endl;
    }

    const std::vector<GLuint>& BindingPointManager::getUsedBindingPoints(BufferTarget bufferType) const {
        return usedBindingPoints.at(bufferType);
    }

    void BindingPointManager::initializeAvailableBindingPoints() {
        const int maxSSBOBindings = 16; // Typically 16
        const int maxUBOBindings = 16; // Typically 16
        const int maxVBOBindings = 16; // Arbitrary limit for VBOs
        const int maxEBOBindings = 16; // Arbitrary limit for EBOs

        for (int i = 0; i < maxSSBOBindings; ++i) {
            availableBindingPoints[BufferTarget::Shader_Storage_Buffer].push(i);
        }
        for (int i = 0; i < maxUBOBindings; ++i) {
            availableBindingPoints[BufferTarget::Uniform_Buffer].push(i);
        }
        for (int i = 0; i < maxVBOBindings; ++i) {
            availableBindingPoints[BufferTarget::Array_Buffer].push(i);
        }
        for (int i = 0; i < maxEBOBindings; ++i) {
            availableBindingPoints[BufferTarget::Element_Array_Buffer].push(i);
        }
        // Add other buffer types if needed
    }


}