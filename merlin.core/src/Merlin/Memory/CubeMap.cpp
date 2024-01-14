#include "glpch.h"
#include "CubeMap.h"
#include "Merlin/Core/Log.h"

#include <stb_image.h>

namespace Merlin::Memory {

    CubeMap::CubeMap(std::string right, std::string left, std::string top, std::string bottom, std::string front, std::string back) {
        _textureID = LoadCubeMap({ right, left, top, bottom, front, back });
    }

    CubeMap::CubeMap(const std::vector<std::string>& faces) {
        _textureID = LoadCubeMap(faces);
    }

    CubeMap::~CubeMap() {
        glDeleteTextures(1, &_textureID);
    }

    void CubeMap::Bind(unsigned int unit) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);
    }

    void CubeMap::Unbind() const {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    GLuint CubeMap::LoadCubeMap(const std::vector<std::string>& faces) {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        // Flips the image so it appears right side up
        stbi_set_flip_vertically_on_load(true);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++) {
            unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                GLenum format = GL_RGB;
                if (nrChannels == 4) format = GL_RGBA;
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else {
                Console::error("CubeMap") << "Cubemap texture failed to load at path: " << faces[i] << Console::endl;
                stbi_image_free(data);
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }
}