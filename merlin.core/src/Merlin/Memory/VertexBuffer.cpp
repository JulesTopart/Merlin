#include "glpch.h"
#include "VertexBuffer.h"
#include "../Core/Console.h"

namespace Merlin::Memory{
	VertexBufferLayout Vertex::GetLayout() {
		VertexBufferLayout layout;
		layout.Push<float>(3); //Vertex pos
		layout.Push<float>(3); //Vertex normal
		layout.Push<float>(3); //Vertex color
		layout.Push<float>(2); //Texture coordinates
		return layout;
	}
}