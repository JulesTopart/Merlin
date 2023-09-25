#include "glpch.h"
#include "VertexArray.h"

namespace Merlin::Memory {

	VertexArray::VertexArray() {
		ArrayID = -1;
	}

	VertexArray::~VertexArray() {
		glDeleteVertexArrays(1, &ArrayID);
		Console::trace("VertexArray") << "VertexArray " << ArrayID << " deleted. " << Console::endl;
	}

	void VertexArray::Generate() {
		if (ArrayID != -1) return; //Already exist
		glCreateVertexArrays(1, &ArrayID);
		Console::trace("VertexArray") << "VertexArray " << ArrayID << " generated. " << Console::endl;
	}

	void VertexArray::Bind() const{

		glBindVertexArray(ArrayID);
	}

	void VertexArray::Unbind() const{
		glBindVertexArray(0);
	}

	

}