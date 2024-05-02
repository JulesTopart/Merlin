#pragma once
#include "merlin/core/core.h"
#include "merlin/core/log.h"
#include "merlin/graphics/mesh.h"

namespace Merlin {

	enum class FileType {
		HDR,
		PNG,
		JPG,
		OBJ,
		STL,
		GEOM,
		GLTF,
		UNKNOWN
	};

	FileType getFileType(const std::string& file_path);
	std::string getFileExtension(const std::string& filepath);
	std::string getFileName(const std::string& filepath);

	template<typename T>
	void debugVector(std::vector<T>& vec);


}

template<typename T>
void  Merlin::debugVector(std::vector<T>& vec) {
	Console::info("Vector") << " : " << Console::endl << "[";
	for (GLuint i = 0; i < ((vec.size() > 100) ? 100 : vec.size() - 1); i++) {
		Console::print() << vec[i] << ", ";
	}
	if (vec.size() > 100) Console::print() << "..., ";
	Console::print() << vec[vec.size() - 1] << "]" << Console::endl << Console::endl;

}