#include "glpch.h"
#include "UniformBuffer.h"


namespace Merlin::Memory {

	template<typename T>
	UniformBuffer<T>::UniformBuffer(std::vector<T> data) : BufferObject(GL_UNIFORM_BUFFER) {

	}

	template<typename T>
	UniformBuffer<T>::~UniformBuffer(){}

}