#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Memory/BufferObject.h"

namespace Merlin::Memory {

    template<typename T>
	class UniformBuffer : BufferObject{
		UniformBuffer(std::vector<T> data);
		~UniformBuffer();
	};

}


