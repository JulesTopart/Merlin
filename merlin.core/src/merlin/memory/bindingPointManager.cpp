#include "glpch.h"
#include "bindingPointManager.h"

namespace Merlin {

    GLuint BindingPointManager::allocateBindingPoint(BufferType bufferType) {
        if (availableBindingPoints[bufferType].empty()) {
            throw std::runtime_error("No available binding points for the given buffer type");
        }
        GLuint bindingPoint = availableBindingPoints[bufferType].front();
        availableBindingPoints[bufferType].pop();
        usedBindingPoints[bufferType].push_back(bindingPoint);
        Console::trace("BindingPointManager") << bindingPoint << " allocated" << Console::endl;
        return bindingPoint;
    }

void BindingPointManager::releaseBindingPoint(BufferType bufferType, GLuint bindingPoint) {
    auto& points = usedBindingPoints[bufferType];
    points.erase(std::remove(points.begin(), points.end(), bindingPoint), points.end());
    // Push the released binding point to the front of the queue
    std::queue<GLuint> tempQueue;
    tempQueue.push(bindingPoint);
    while (!availableBindingPoints[bufferType].empty()) {
        tempQueue.push(availableBindingPoints[bufferType].front());
        availableBindingPoints[bufferType].pop();
    }
    availableBindingPoints[bufferType] = tempQueue;
    Console::trace("BindingPointManager") << bindingPoint  << " freed" << Console::endl;
}

    const std::vector<GLuint>& BindingPointManager::getUsedBindingPoints(BufferType bufferType) const {
        return usedBindingPoints.at(bufferType);
    }


    void BindingPointManager::initializeAvailableBindingPoints() {
        const int maxSSBOBindings = 16; // Typically 16
        const int maxUBOBindings = 16; // Typically 16
        const int maxVBOBindings = 16; // Arbitrary limit for VBOs
        const int maxEBOBindings = 16; // Arbitrary limit for EBOs

        for (int i = 0; i < maxSSBOBindings; ++i) {
            availableBindingPoints[BufferType::SHADER_STORAGE_BUFFER].push(i);
        }
        for (int i = 0; i < maxUBOBindings; ++i) {
            availableBindingPoints[BufferType::UNIFORM_BUFFER].push(i);
        }
        for (int i = 0; i < maxVBOBindings; ++i) {
            availableBindingPoints[BufferType::VERTEX_BUFFER].push(i);
        }
        for (int i = 0; i < maxEBOBindings; ++i) {
            availableBindingPoints[BufferType::INDEX_BUFFER].push(i);
        }
        // Add other buffer types if needed
    }


}