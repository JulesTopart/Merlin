#include "glpch.h"
#include "textureLoader.h"
#include "util.h"

ImageData TextureLoader::loadImageData(const std::string& filepath) {
    FileType ft = getFileType(filepath);
    ImageData data;

    switch (ft) {
    case Merlin::FileType::HDR:
        parseHDR(filepath, data);
        break;
    case Merlin::FileType::PNG:
        parsePNG_JPG(filepath, data);
        break;
    case Merlin::FileType::JPG:
        break;
    default:
        break;
    }

    if (data.bytes == nullptr) {
        Console::error("TextureLoader") << "could not load tecture " << filepath << Console::endl;
        Console::error("TextureLoader") << " stb error : " << stbi_failure_reason() << Console::endl;
        throw std::runtime_error("cannot load image");
        stbi_image_free(data.bytes);
        return data;
    }

    return data;
}


Shared<Texture2D> TextureLoader::loadTexture(const std::string& filepath, TextureType t) {
    return Texture2D::create(filepath, t);
}

void Merlin::TextureLoader::parseHDR(const std::string& filepath, ImageData& data){
    data.bytes = stbi_loadf(filepath.c_str(), &data.width, &data.height, &data.channels,0);
    data.bits = 32; //32bits per channel
    data.dataType = GL_FLOAT; // Specify that the data type is float
   
}

void Merlin::TextureLoader::parsePNG_JPG(const std::string& filepath, ImageData& data){
    data.bytes = stbi_load(filepath.c_str(), &data.width, &data.height, &data.channels, 0);
    data.dataType = GL_UNSIGNED_BYTE; // Data is stored as unsigned bytes
    data.bits = 8;
}

