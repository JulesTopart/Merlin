#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Memory/BufferObject.h"
#include "Merlin/Graphics/ShaderBase.h"

using namespace Merlin::Graphics;

namespace Merlin::Memory {

	class UniformBuffer : BufferObject{
		UniformBuffer(std::string name);
		~UniformBuffer();

		void SetBindingPoint(GLuint binding);
		void Attach(const ShaderBase& sh); //Link the program buffer to the right binding point

		inline GLuint binding() const { return _binding; }
		inline std::string name() const { return _name; }

		static void copy(Shared<UniformBuffer> origin, Shared<UniformBuffer> target, GLsizeiptr size);

	private:
		std::string _name;
		GLuint _binding = 0;
	};

	typedef UniformBuffer UBO;
}


