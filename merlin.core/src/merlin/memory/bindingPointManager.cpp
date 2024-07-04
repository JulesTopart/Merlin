#include "glpch.h"
#include "bindingPointManager.h"

namespace Merlin {

    BindingPointHandle::BindingPointHandle(BufferTarget target, GLuint bindingPoint, bool autoRelease)
        : target(target), bindingPoint(bindingPoint), autoRelease(autoRelease) {}

    BindingPointHandle::BindingPointHandle(BindingPointHandle&& other) noexcept
        : target(other.target), bindingPoint(other.bindingPoint), autoRelease(other.autoRelease) {
        other.bindingPoint = INVALID_BINDING_POINT;
    }

    BindingPointHandle& BindingPointHandle::operator=(BindingPointHandle&& other) noexcept {
        if (this != &other) {
            release();
            target = other.target;
            bindingPoint = other.bindingPoint;
            autoRelease = other.autoRelease;
            other.bindingPoint = INVALID_BINDING_POINT;
        }
        return *this;
    }

    BindingPointHandle::~BindingPointHandle() {
        release();
    }

    GLuint BindingPointHandle::get() const {
        return bindingPoint;
    }

    void BindingPointHandle::release() {
        if (bindingPoint != INVALID_BINDING_POINT && autoRelease) {
            BindingPointManager::instance().releaseBindingPoint(target, bindingPoint);
        }
    }

    BindingPointManager::BindingPointManager() {
        initializeAvailableBindingPoints();
    }

    BindingPointHandle BindingPointManager::allocateBindingPoint(BufferTarget bufferType, bool autoRelease) {
        if (availableBindingPoints[bufferType].empty()) {
            Console::error("BindingPointManager") << "No available binding points for the given buffer type" << Console::endl;
            throw std::runtime_error("No available binding points for the given buffer type");
        }
        GLuint bindingPoint = availableBindingPoints[bufferType].front();
        availableBindingPoints[bufferType].pop();
        Console::trace("BindingPointManager") << bindingPoint << " allocated" << Console::endl;
        return BindingPointHandle(bufferType, bindingPoint, autoRelease);
    }

    void BindingPointManager::releaseBindingPoint(BufferTarget bufferType, GLuint bindingPoint) {
        std::queue<GLuint> tempQueue;
        tempQueue.push(bindingPoint);
        while (!availableBindingPoints[bufferType].empty()) {
            tempQueue.push(availableBindingPoints[bufferType].front());
            availableBindingPoints[bufferType].pop();
        }
        availableBindingPoints[bufferType] = tempQueue;
        Console::trace("BindingPointManager") << bindingPoint << " freed" << Console::endl;
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
    }



}