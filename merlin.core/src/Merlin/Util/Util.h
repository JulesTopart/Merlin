#pragma once
#include "Merlin/Core/Core.h"

namespace Merlin::Utils {

	enum class FileType {
		OBJ,
		STL,
		GEOM,
		GLTF,
		UNKNOWN
	};

	FileType GetFileType(const std::string& file_path);
	std::string GetFileExtension(const std::string& filepath);
	std::string GetFileName(const std::string& filepath);

}