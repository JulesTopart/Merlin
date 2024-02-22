#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Memory/BufferObject.h"

#include <vector>

namespace Merlin {
	class IndexBuffer : public BufferObject<GLuint>{
	public:
		IndexBuffer(std::vector<GLuint>& indices);
		~IndexBuffer();
	};

	typedef IndexBuffer EBO;
	typedef std::vector<GLuint> Indices;
}