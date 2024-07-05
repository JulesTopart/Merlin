#include "glpch.h"
#include "merlin/memory/bufferObject.h"
#include "merlin/memory/bindingPointManager.h"

namespace Merlin {
    void AbstractBufferObject::releaseBindingPoint() {
        if(m_bindingPoint < 16) BindingPointManager::instance().releaseBindingPoint(m_target, m_bindingPoint);
        m_bindingPoint = -1;
        return;
    }


    void AbstractBufferObject::setBindingPoint(GLuint point) {
        m_bindingPoint = point;

        switch (m_target) {
            case BufferTarget::Atomic_Counter_Buffer:
            case BufferTarget::Transform_Feedback_Buffer:
            case BufferTarget::Uniform_Buffer:
            case BufferTarget::Shader_Storage_Buffer:
                glBindBufferBase(static_cast<GLenum>(m_target), m_bindingPoint, id());
            break;
        }
    }
}