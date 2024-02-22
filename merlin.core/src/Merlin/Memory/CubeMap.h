#pragma once
#include "Merlin/Core/Core.h"

namespace Merlin{
	class CubeMap{
	public:
		CubeMap(std::string right, std::string left, std::string top, std::string bottom, std::string front, std::string back);
		CubeMap(const std::vector<std::string>& faces);
		~CubeMap();

		void Bind(unsigned int unit) const;
		void Unbind() const;

	private:
		GLuint LoadCubeMap(const std::vector<std::string>& faces);

		GLuint _textureID;

	};
}

