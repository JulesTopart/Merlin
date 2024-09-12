
#include "glpch.h"
#include "merlin/memory/vao.h"
#include "merlin/core/console.h"

namespace Merlin {

    VertexArray::VertexArray() : GLObject(create(), destroy) {}

    GLuint VertexArray::create() {
        GLuint id;
        glCreateVertexArrays(1, &id);
        Console::success("VertexArray") << "VertexArray " << id << " generated. " << Console::endl;
        return id;
    }

    void VertexArray::destroy(GLuint ID) {
        Console::trace("VertexArray") << "VertexArray " << ID << " deleted. " << Console::endl;
        glDeleteVertexArrays(1, &ID);
    }

    void VertexArray::bindBuffer(IndexBuffer& ib) {
        glVertexArrayElementBuffer(id(), ib.id());
    }
}