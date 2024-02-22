#include "glpch.h"
#include "VertexArray.h"

namespace Merlin {

	VertexArray::VertexArray() {
		glCreateVertexArrays(1, &ArrayID);
		Bind();
		Console::success("VertexArray") << "VertexArray " << ArrayID << " generated. " << Console::endl;
	}

	VertexArray::~VertexArray() {
		glDeleteVertexArrays(1, &ArrayID);
		Console::trace("VertexArray") << "VertexArray " << ArrayID << " deleted. " << Console::endl;
	}

	void VertexArray::Bind() const{
		if (ArrayID == -1) Console::error("VertexArray") << "VAO is not generated yet" << Console::endl; //Already exist
		else glBindVertexArray(ArrayID);
	}

	void VertexArray::Unbind() const{
		glBindVertexArray(0);
	}

	

}