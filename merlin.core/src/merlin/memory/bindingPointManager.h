#pragma once
#include "merlin/core/core.h"

#include <unordered_map>
#include <queue>
#include <vector>
#include <algorithm>
#include <stdexcept>

#include "merlin/memory/bufferObject.h"

namespace Merlin {
    class BindingPointHandle {
    public:
        BindingPointHandle(BufferTarget target, GLuint bindingPoint, bool autoRelease = true);
        BindingPointHandle(BindingPointHandle&& other) noexcept;
        BindingPointHandle& operator=(BindingPointHandle&& other) noexcept;
        ~BindingPointHandle();

        GLuint get() const;

        BindingPointHandle(const BindingPointHandle&) = delete;
        BindingPointHandle& operator=(const BindingPointHandle&) = delete;

    private:
        void release();

        static constexpr GLuint INVALID_BINDING_POINT = static_cast<GLuint>(-1);

        BufferTarget target;
        GLuint bindingPoint;
        bool autoRelease;
    };

    class BindingPointManager {
        BindingPointManager();
        SINGLETON(BindingPointManager)

    public:
        BindingPointHandle allocateBindingPoint(BufferTarget target, bool autoRelease = true);
        void releaseBindingPoint(BufferTarget target, GLuint bindingPoint);

    private:
        void initializeAvailableBindingPoints();

        std::unordered_map<BufferTarget, std::queue<GLuint>> availableBindingPoints;
    };

}

