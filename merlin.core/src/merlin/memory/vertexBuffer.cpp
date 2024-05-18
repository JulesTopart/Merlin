#include "glpch.h"
#include "vertexBuffer.h"
#include "merlin/core/console.h"

namespace Merlin{
	VertexBufferLayout Vertex::getLayout() {
		VertexBufferLayout layout;
		layout.push<float>(3); //Vertex pos
		layout.push<float>(3); //Vertex normal
		layout.push<float>(3); //Vertex color
		layout.push<float>(2); //Texture coordinates
		layout.push<float>(3); //tangent
		layout.push<float>(3); //bitangent
		return layout;
	}
}