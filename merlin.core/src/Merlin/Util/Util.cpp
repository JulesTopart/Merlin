
#include "glpch.h"
#include "Merlin/Util/Util.h"


namespace Merlin {	

	FileType GetFileType(const std::string& file_path) {
		std::string extension = GetFileExtension(file_path);
		if (extension == "obj") {
			return FileType::OBJ;
		}
		else if (extension == "stl") {
			return FileType::STL;
		}
		else if (extension == "geom") {
			return FileType::GEOM;
		}
		else if (extension == "gltf") {
			return FileType::GLTF;
		}
		else {
			// Unknown file type
			return FileType::UNKNOWN;
		}
	}

	std::string GetFileExtension(const std::string& filepath) {
		size_t pos = filepath.find_last_of(".");
		if (pos == std::string::npos) {
			// No extension found
			return "";
		}
		return filepath.substr(pos + 1);
	}

	std::string GetFileName(const std::string& filepath)
	{
		// Find the last occurrence of the directory separator character
		std::size_t pos = filepath.find_last_of("\\/");

		// If the separator was not found, return the entire filepath
		if (pos == std::string::npos) {
			return filepath;
		}

		// Return the part of the filepath after the separator
		return filepath.substr(pos + 1);
	}


}