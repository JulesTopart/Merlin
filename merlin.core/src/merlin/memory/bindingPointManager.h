#pragma once
#include "merlin/core/core.h"

#include <unordered_map>
#include <queue>
#include <vector>
#include <algorithm>
#include <stdexcept>

#include "merlin/memory/bufferObject.h"

namespace Merlin {
    class BindingPointManager {
        SINGLETON(BindingPointManager)
        BindingPointManager() { initializeAvailableBindingPoints(); }


    public:
        GLuint allocateBindingPoint(BufferTarget bufferTarget, GLuint bufferID);
        void releaseBindingPoint(BufferTarget bufferTarget, GLuint bindingPoint);
        const std::vector<GLuint>& getUsedBindingPoints(BufferTarget bufferTarget) const;
        void resetBindings();

    private:
        void initializeAvailableBindingPoints();

        std::unordered_map<BufferTarget, std::queue<GLuint>> availableBindingPoints;
        std::unordered_map<BufferTarget, std::vector<GLuint>> usedBindingPoints;
        std::unordered_map<GLuint, GLuint> bufferToBindingPoint; // Maps buffer ID to binding point

    };
}

