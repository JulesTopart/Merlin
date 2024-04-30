#include "glpch.h"
#include "vertexArray.h"

namespace Merlin {

	VertexArray::VertexArray() : GLObject(create(), destroy){}

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

	void VertexArray::bind() const{
		if (id() == -1) Console::error("VertexArray") << "VAO is not generated yet" << Console::endl; //Already exist
		else glBindVertexArray(id());
	}

	void VertexArray::unbind() const{
		glBindVertexArray(0);
	}

	void VertexArray::bindBuffer(IndexBuffer& ib) {
		glVertexArrayElementBuffer(id(), ib.id());
	}
	

}