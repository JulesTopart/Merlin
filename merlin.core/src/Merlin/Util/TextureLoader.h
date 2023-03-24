#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Memory/Texture.h"

using namespace Merlin::Graphics;
using namespace Merlin::Memory;

namespace Merlin::Utils {

	class TextureLoader {
	public:
		// Load a texture from the specified file and return a pointer to a new Texture object
		static Shared<Texture> LoadTexture(const std::string& filepath);

	private:

		enum class FileType {
			OBJ,
			STL,
			GEOM,
			UNKNOWN
		};

		static FileType GetFileType(const std::string& filepath);
		static std::string GetFileExtension(const std::string& filepath);
		static std::string GetFileName(const std::string& filepath);


	};

}