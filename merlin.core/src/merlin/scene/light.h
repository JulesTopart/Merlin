#pragma once
#include "merlin/core/core.h"
#include "merlin/graphics/renderableObject.h"
#include "merlin/memory/frameBuffer.h"
#include "merlin/textures/cubeMap.h"
#include "merlin/utils/primitives.h"
#include "merlin/graphics/mesh.h"
#include "merlin/utils/util.h"
#include <glm/glm.hpp>

namespace Merlin {

    enum class LightType {
        Ambient = 0,
        Point = 1,
        Directional = 2,
        Spot = 3,
        
    };

	class Light : public RenderableObject{
    public:
        Light(const std::string& name, LightType type, const glm::vec3& ambient = glm::vec3(0.05), const glm::vec3& diffuse = glm::vec3(0.7), const glm::vec3& specular = glm::vec3(0.5))
            : RenderableObject(name), ambient_(ambient), diffuse_(diffuse), specular_(specular), type_(type) {
            
            switch (type){
            case Merlin::LightType::Ambient:
                m_mesh = Primitives::createPoint();
                break;
            case Merlin::LightType::Point:
                m_mesh = Primitives::createSphere(2);
                break;
            case Merlin::LightType::Directional:
                m_mesh = Primitives::createCylinder(1,2,20);
                break;
            case Merlin::LightType::Spot:
                m_mesh = Primitives::createCone(1, 2, 20);
                m_mesh->rotate(glm::vec3(90 * DEG_TO_RAD, 0, 0));
                m_mesh->applyMeshTransform();
                break;
            default:
                break;
            }
            
        }


        virtual void attach(int id, Shader& shader) = 0;
        virtual void detach() {};
        virtual void attachShadow(Shader&, float scale = 10){}

        virtual Shared<FBO> shadowFBO() { return nullptr; }
        virtual Shared<TextureBase> shadowMap() { return nullptr; }

        inline void applyRenderTransform(const glm::mat4& transform) { renderTransform = transform; }
        inline const glm::mat4& getRenderTransform() { return renderTransform; }


        inline void setAmbient(float r, float g, float b) { ambient_ = glm::vec3(r,g,b); }
        inline void setAmbient(const glm::vec3& c) { ambient_ = c; }
        inline void setDiffuse(float r, float g, float b) { diffuse_ = glm::vec3(r, g, b);}
        inline void setDiffuse(const glm::vec3& c) { diffuse_ = c;}
        inline void setSpecular(float r, float g, float b) { specular_ = glm::vec3(r, g, b);}
        inline void setSpecular(const glm::vec3& c) { specular_ = c;}

        virtual void setShadowResolution(GLuint res);
        inline GLuint shadowResolution() const { return m_shadowResolution; }


        inline const void setAttenuation(glm::vec3 att) { attenuation_ = att; }
        inline const glm::vec3& attenuation() { return attenuation_; }

        void draw() const;
        inline Mesh& mesh() { return *m_mesh; }
        inline const glm::mat4& meshTransform() const { return m_mesh->transform(); }

        inline const glm::vec3& ambient() const { return ambient_; }
        inline const glm::vec3& diffuse() const { return diffuse_; }
        inline const glm::vec3& specular() const { return specular_; }
        LightType type() const { return type_; }


    protected : 
        GLuint m_shadowResolution = 512;

    private:
        glm::vec3 ambient_;
        glm::vec3 diffuse_;
        glm::vec3 specular_;

        glm::vec3 attenuation_ = glm::vec3(1.0, 0.09f, 0.0032f); //constant, linear, quadratic
        LightType type_;

        inline static Shared<Mesh> m_mesh = nullptr;

        glm::mat4 renderTransform = glm::identity<glm::mat4>();
	};

    class DirectionalLight : public Light {
    public:
        DirectionalLight(const std::string& name, const glm::vec3& direction = glm::vec3(0, 0, -1))
            : Light(name, LightType::Directional), direction_(direction) {
            m_shadowResolution = 512;
            generateShadowMap();
            m_castShadow = true;
        }
        DirectionalLight(const std::string& name, const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
            : Light(name, LightType::Directional, ambient, diffuse, specular), direction_(direction) {
            m_shadowResolution = 512;
            generateShadowMap();
            m_castShadow = true;
        }

        const glm::vec3& direction() const { return direction_; }

        void attach(int id, Shader&) override;
        void detach() override;
        void attachShadow(Shader&, float scale = 10) override;
        inline Shared<FBO> shadowFBO() override { return m_shadowFBO; }
        inline Shared<TextureBase> shadowMap() override { return m_shadowMap; }
        void setShadowResolution(GLuint res) override;

        void generateShadowMap();

        inline Shared<Texture2D> getShadowMap() { return m_shadowMap; }
        inline void setShadowMap(Shared<Texture2D> tex) { m_shadowMap = tex; }

        inline Shared<FrameBuffer> getShadowFBO() { return m_shadowFBO; }
        inline void setShadowFBO(Shared<FrameBuffer> fbo) { m_shadowFBO = fbo; }


        inline const glm::mat4& getLightSpaceMatrix() { return m_lightSpaceMatrix; }


        static Shared<DirectionalLight> create(const std::string& name, const glm::vec3& direction = glm::vec3(0, 0, -1)) {
            return createShared<DirectionalLight>(name, direction);
        }

        static Shared<DirectionalLight> create(const std::string& name, const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular) {
            return createShared<DirectionalLight>(name, direction, ambient, diffuse, specular);
        }


    private:
        glm::vec3 direction_;
        Shared<Texture2D> m_shadowMap = nullptr;
        Shared<FrameBuffer> m_shadowFBO = nullptr;
        glm::mat4 m_lightSpaceMatrix;
    };

    class PointLight : public Light {
    public:

        PointLight(const std::string& name, const glm::vec3& position = glm::vec3(0))
            : Light(name, LightType::Point) {
            setPosition(position);
            generateShadowMap();
            m_castShadow = true;
        }

        PointLight(const std::string& name, const glm::vec3& position, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
            : Light(name, LightType::Point, ambient, diffuse, specular) {
            setPosition(position);
            generateShadowMap();
            m_castShadow = true;
        }

        void generateShadowMap();
        void detach() override;
        void attach(int id, Shader&) override;
        void attachShadow(Shader&, float scale = 10) override;
        void setShadowResolution(GLuint res) override;
        inline Shared<FBO> shadowFBO() override { return m_shadowFBO; }
        inline Shared<TextureBase> shadowMap() override { return m_shadowMap; }

        Shared<CubeMap> getShadowMap() { return m_shadowMap; }
        void setShadowMap(Shared<CubeMap> tex) { m_shadowMap = tex; }

        Shared<FrameBuffer> getShadowFBO() { return m_shadowFBO; }
        void setShadowFBO(Shared<FrameBuffer> fbo) { m_shadowFBO = fbo; }


        static Shared<PointLight> create(const std::string& name, const glm::vec3& position = glm::vec3(0)) {
            return createShared<PointLight>(name, position);
        }

        static Shared<PointLight> create(const std::string& name, const glm::vec3& position, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular) {
            return createShared<PointLight>(name, position, ambient, diffuse, specular);
        }


    private:
        Shared<CubeMap> m_shadowMap = nullptr;
        Shared<FrameBuffer> m_shadowFBO = nullptr;
        glm::mat4 m_lightSpaceMatrix = glm::mat4(1);
        std::vector<glm::mat4> m_shadowTransforms;
    };

    class AmbientLight : public Light {
    public:

        AmbientLight(const std::string& name)
            : Light(name, LightType::Ambient) {}

        AmbientLight(const std::string& name, const glm::vec3& ambient)
            : Light(name, LightType::Ambient, ambient) {}

        void attach(int id, Shader&) override;

        static Shared<AmbientLight> create(const std::string& name) {
            return createShared<AmbientLight>(name);
        }

        static Shared<AmbientLight> create(const std::string& name, const glm::vec3& ambient) {
            return createShared<AmbientLight>(name, ambient);
        }
    };

    class SpotLight : public Light {
    public:

        SpotLight(const std::string& name, const glm::vec3& position = glm::vec3(0), const glm::vec3& direction = glm::vec3(0, 0, -1), float cutOff = 50.0f)
            : Light(name, LightType::Spot), direction_(direction), cutOff_(cutOff) {
            setPosition(position);
            m_shadowResolution = 512;
            generateShadowMap();
            setAttenuation(glm::vec3(0.1, 0.001, 0.0008));
            m_castShadow = true;
        }

        SpotLight(const std::string& name, const glm::vec3& position, const glm::vec3& direction, float cutOff, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
            : Light(name, LightType::Spot, ambient, diffuse, specular), direction_(direction), cutOff_(cutOff) {
            setPosition(position);
            m_shadowResolution = 512;
            generateShadowMap();
            setAttenuation(glm::vec3(0.1, 0.001, 0.0008));
            m_castShadow = true;
        }

        const glm::vec3& direction() const { return direction_; }
        float cutOff() const { return cutOff_; }

        void attach(int id, Shader&) override;
        void detach() override;

        void attachShadow(Shader&, float scale = 10) override;
        inline Shared<FBO> shadowFBO() override { return m_shadowFBO; }
        inline Shared<TextureBase> shadowMap() override { return m_shadowMap; }
        void setShadowResolution(GLuint res) override;

        void generateShadowMap();

        inline Shared<Texture2D> getShadowMap() { return m_shadowMap; }
        inline void setShadowMap(Shared<Texture2D> tex) { m_shadowMap = tex; }

        inline Shared<FrameBuffer> getShadowFBO() { return m_shadowFBO; }
        inline void setShadowFBO(Shared<FrameBuffer> fbo) { m_shadowFBO = fbo; }


        inline const glm::mat4& getLightSpaceMatrix() { return m_lightSpaceMatrix; }



        static Shared<SpotLight> create(const std::string& name, const glm::vec3& position = glm::vec3(0), const glm::vec3& direction = glm::vec3(0, 0, -1), float cutOff = 50.0f){
            return createShared<SpotLight>(name, position , direction,cutOff);
        }

        static Shared<SpotLight> create(const std::string& name, const glm::vec3& position, const glm::vec3& direction, float cutOff, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular) {
            return createShared<SpotLight>(name, position, direction, cutOff, ambient, diffuse, specular);
        }

    private:
        glm::vec3 direction_;

        float cutOff_; // Cut-off angle for the spot light

        Shared<Texture2D> m_shadowMap = nullptr;
        Shared<FrameBuffer> m_shadowFBO = nullptr;
        glm::mat4 m_lightSpaceMatrix;
    };


    typedef Shared<AmbientLight> AmbientLight_Ptr;
    typedef Shared<SpotLight> SpotLight_Ptr;
    typedef Shared<DirectionalLight> DirectionalLight_Ptr;
    typedef Shared <PointLight> PointLight_Ptr;

}

