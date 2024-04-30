#pragma once
#include "merlin/core/core.h"

namespace Merlin{
	class CubeMap{
	public:
		CubeMap(std::string right, std::string left, std::string top, std::string bottom, std::string front, std::string back);
		CubeMap(const std::vector<std::string>& faces);
		~CubeMap();

		void bind(unsigned int unit) const;
		void unbind() const;

	private:
		GLuint loadCubeMap(const std::vector<std::string>& faces);

		GLuint _textureID;

	};
}

