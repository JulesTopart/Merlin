#pragma once
#include "merlin/core/core.h"
#include "merlin/memory/bufferObject.h"

#include <queue>

namespace Merlin {
	class BindingPointManager{
        SINGLETON(BindingPointManager)
        BindingPointManager() { initializeAvailableBindingPoints(); };

    public:

        GLuint allocateBindingPoint(BufferType bufferType);
        void releaseBindingPoint(BufferType bufferType, GLuint bindingPoint);
        const std::vector<GLuint>& getUsedBindingPoints(BufferType bufferType) const;

    private:
        void initializeAvailableBindingPoints();
    
        std::unordered_map<BufferType, std::queue<GLuint>> availableBindingPoints;
        std::unordered_map<BufferType, std::vector<GLuint>> usedBindingPoints;
    };
}

