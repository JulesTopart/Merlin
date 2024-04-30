#pragma once
#include "merlin/core/core.h"
#include "merlin/memory/texture.h"

using namespace Merlin;
using namespace Merlin;

namespace Merlin {

	class TextureLoader {
	public:
		// Load a texture from the specified file and return a pointer to a new Texture object
		static Shared<Texture> loadTexture(const std::string& filepath);

	private:

		enum class FileType {
			OBJ,
			STL,
			GEOM,
			UNKNOWN
		};

		static FileType getFileType(const std::string& filepath);
		static std::string getFileExtension(const std::string& filepath);
		static std::string getFileName(const std::string& filepath);


	};

}