#pragma once
#include "merlin/core/core.h"
#include "merlin/memory/bufferObject.h"

#include <vector>

namespace Merlin {
	class IndexBuffer : public BufferObject<GLuint>{
	public:
		IndexBuffer(const std::vector<GLuint>& indices);
		~IndexBuffer();
	};

	inline IndexBuffer::IndexBuffer(const std::vector<GLuint>& indices) : BufferObject<GLuint>(BufferTarget::Element_Array_Buffer) {
		this->allocate(indices.size(), BufferUsage::StaticDraw);
		this->write(indices);
		Console::trace("IndexBuffer") << "IndexBuffer " << this->id() << " allocated. " << Console::endl;
	}

	inline IndexBuffer::~IndexBuffer() {
		Console::trace("IndexBuffer") << "IndexBuffer " << this->id() << " destroyed. " << Console::endl;
	}

	// Shorter aliases
	typedef IndexBuffer EBO;
	typedef std::shared_ptr<IndexBuffer> EBO_Ptr;
	typedef IndexBuffer IBO;
	typedef std::shared_ptr<IndexBuffer> IBO_Ptr;
	typedef std::vector<GLuint> Indices;
}