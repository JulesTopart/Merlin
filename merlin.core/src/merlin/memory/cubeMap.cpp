#include "glpch.h"
#include "cubeMap.h"
#include "merlin/core/log.h"
#include "merlin/utils/textureLoader.h"

#include <stb_image.h>

namespace Merlin {

    CubeMap::CubeMap(TextureType t) : TextureBase(GL_TEXTURE_CUBE_MAP, t, TextureClass::CUBE_MAP){ 

    }

    void CubeMap::generateMipmap() const {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    void CubeMap::setInterpolationMode(GLuint settingMin, GLuint settingMag) {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, settingMin);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, settingMag);
    }

    void CubeMap::setRepeatMode(GLuint _wrapS, GLuint _wrapT, GLuint _wrapR) {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, _wrapS);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, _wrapT);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, _wrapR);
    }

    void CubeMap::setBorderColor4f(float colors[4]) {
        glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, colors);
    }

    void CubeMap::setBorderColor4f(float R, float G, float B, float A) {
        float colors[4] = { R,G,B,A };
        glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, colors);
    }

    void CubeMap::reserve(GLuint width, GLuint height, GLuint channels, GLuint bits){
        
        m_width = width;
        m_height = height;
        m_format = GL_RGB;
        m_internalFormat = GL_RGB8;
        m_dataType = GL_UNSIGNED_BYTE;

        // Determine format and internal format based on channels and bits
        switch (channels) {
        case 1:
            m_format = GL_RED;
            m_internalFormat = (bits == 32) ? GL_R32F : GL_R8;
            break;
        case 3:
            m_format = GL_RGB;
            m_internalFormat = (bits == 32) ? GL_RGB32F : GL_RGB8;
            break;
        case 4:
            m_format = GL_RGBA;
            m_internalFormat = (bits == 32) ? GL_RGBA32F : GL_RGBA8;
            break;
        }
        
        for (unsigned int i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_internalFormat, width, height, 0, m_format, m_dataType, nullptr);
        }
    }

    void CubeMap::reserve(GLuint width, GLuint height, GLenum format, GLenum internalFormat, GLenum type) {

        m_width = width;
        m_height = height;
        m_format = format;
        m_internalFormat = internalFormat;
        m_dataType = type;

        for (unsigned int i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_internalFormat, width, height, 0, m_format, m_dataType, nullptr);
        }
        Console::print() << "t";
    }

    void CubeMap::resize(GLsizei width, GLsizei height) {
        // Update the dimensions of the texture
        m_width = width;
        m_height = height;

        for (unsigned int i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_internalFormat, width, height, 0, m_format, m_dataType, nullptr);
        }
    }

    Shared<CubeMap> CubeMap::create(GLuint width, GLuint height, TextureType t){
        Shared<CubeMap> cm = createShared<CubeMap>(t);
        cm->bind();
        if (t == TextureType::SHADOW || t == TextureType::DEPTH) {
            cm->reserve(width, height, GL_DEPTH_STENCIL, GL_DEPTH24_STENCIL8, GL_UNSIGNED_INT_24_8);
            cm->setInterpolationMode(GL_NEAREST, GL_NEAREST);
            cm->setRepeatMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        }
        else {
            ChannelsProperty cp = TextureBase::getChannelsProperty(t);
            cm->reserve(width, height, cp.channels, cp.bits);
        }

        cm->unbind();
        return cm;
    }

    Shared<CubeMap> CubeMap::create(const std::vector<std::string>& paths, TextureType t){
        Shared<CubeMap> cm = createShared<CubeMap>(t);
        cm->autoSetUnit();
        cm->bind();
        cm->loadFromFiles(paths);
        cm->setInterpolationMode(GL_NEAREST, GL_NEAREST);
        cm->setRepeatMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        cm->unbind();
        return cm;
    }

    void CubeMap::loadFromFiles(const std::vector<std::string>& faces) {
        if (faces.size() < 6) {
            Console::error("CubeMap") << "Not enough texture provided" << Console::endl;
                return;
        }
        
        for (unsigned int i = 0; i < faces.size(); i++) {
            ImageData data = TextureLoader::loadImageData(faces[i]);

            if (data.bytes) {
               
                if (i == 0) {
                    m_width = data.width;
                    m_height = data.height;
                    m_format = GL_RGB;
                    m_internalFormat = GL_RGB8;
                    m_dataType = data.dataType;

                    // Determine format and internal format based on channels and bits
                    switch (data.channels) {
                    case 1:
                        m_format = GL_RED;
                        m_internalFormat = (data.bits == 32) ? GL_R32F : GL_R8;
                        break;
                    case 3:
                        m_format = GL_RGB;
                        m_internalFormat = (data.bits == 32) ? GL_RGB32F : GL_RGB8;
                        break;
                    case 4:
                        m_format = GL_RGBA;
                        m_internalFormat = (data.bits == 32) ? GL_RGBA32F : GL_RGBA8;
                        break;
                    }
                }
                Console::success("CubeMap") << "Cubemap texture loaded at path: " << faces[i] << Console::endl;
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_internalFormat, m_width, m_height, 0, m_format, m_dataType, data.bytes);
                stbi_image_free(data.bytes);
            }
            else {
                Console::error("CubeMap") << "Cubemap texture failed to load at path: " << faces[i] << Console::endl;
                stbi_image_free(data.bytes);
            }
        }
    }

}