#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Memory/BufferObject.h"
#include "Merlin/Graphics/ShaderBase.h"

using namespace Merlin::Graphics;

namespace Merlin::Memory {
	class ShaderStorageBuffer : public BufferObject{
	public:
		ShaderStorageBuffer(std::string name);
		~ShaderStorageBuffer();

		void SetBindingPoint(GLuint binding);
		void Attach(const ShaderBase& sh); //Link the program buffer to the right binding point

		inline GLuint binding() const { return _binding; }
		inline std::string name() const { return _name; }

		static void copy(Shared<ShaderStorageBuffer> origin, Shared<ShaderStorageBuffer> target, GLsizeiptr size);

	private:
		std::string _name;
		GLuint _binding = 0;
	};

	typedef ShaderStorageBuffer SSBO;
}

