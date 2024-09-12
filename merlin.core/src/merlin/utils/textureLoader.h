#pragma once
#include "merlin/core/core.h"
#include "merlin/textures/texture.h"

using namespace Merlin;
using namespace Merlin;

namespace Merlin {

	class TextureLoader {
	public:
		// Load a texture from the specified file and return a pointer to a new Texture object
		static Shared<Texture2D> loadTexture(const std::string& filepath, TextureType  t = TextureType::ALBEDO);
		static ImageData loadImageData(const std::string& filepath, bool flipped = false);


	private:
		static void parseHDR(const std::string& filepath, ImageData& data);
		static void parsePNG_JPG(const std::string& filepath, ImageData& data);	};

}