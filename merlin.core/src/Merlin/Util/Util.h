#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Core/Log.h"
#include "Merlin/Graphics/Mesh.h"

namespace Merlin {

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