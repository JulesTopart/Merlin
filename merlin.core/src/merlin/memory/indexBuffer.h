#pragma once
#include "merlin/core/core.h"
#include "merlin/memory/bufferObject.h"

#include <vector>

namespace Merlin {
	class IndexBuffer : public BufferObject<GLuint>{
	public:
		IndexBuffer(std::vector<GLuint>& indices);
		~IndexBuffer();
	};

	typedef IndexBuffer EBO;
	typedef IndexBuffer IBO;
	typedef std::vector<GLuint> Indices;
}