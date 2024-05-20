#pragma once
#include "merlin/core/core.h"

#include "merlin/shaders/shader.h"
#include "merlin/events/applicationevent.h"

using namespace Merlin;

namespace Merlin {

	enum class Projection {
		Orthographic, 
		Perspective
		//Isometric
	};

	class Camera{
	public:
		Camera();
		Camera(float width, float height, Projection = Projection::Perspective );

		//Event
		void onEvent(Event& e);

		void translate(float dx, float dy, float dz);//3D
		void translate(float dx, float dy);//2D
		void translate(glm::vec3 du3d);
		void translate(glm::vec2 du2d);

		void rotate(float dRx, float dRy, float dRz);
		void rotate(glm::vec3 dR);

		bool isOrthoGraphic() const { return _projection == Projection::Orthographic; }
		//bool () const { return _projection == Projection::Orthographic; }

		void reset();
		void resetProjection();
		void restoreViewport() const;

		inline void setZoom(float zoom) { _zoom = zoom; resetProjection();}
		inline void setFOV(float fov) { _fov = fov; resetProjection(); }
		inline void setNearPlane(float np) { _nearPlane = np; resetProjection();}
		inline void setFarPlane(float fp) { _farPlane = fp; resetProjection();}

		inline const float getZoom() const { return _zoom; }
		inline const float getAspectRatio() const { return _AspectRatio; }
		inline const int width() const { return _width; }
		inline const int height() const { return _height; }

		const glm::vec3& getPosition() const { return _Position; }
		void setPosition(const glm::vec3& position) { _Position = position; recalculateViewMatrix(); }
		const glm::vec3& getTarget() const { return _Target; }
		void setTarget(const glm::vec3& target) { _Target = target; recalculateViewMatrix(); }
		glm::vec3 getRotation() const { return _Rotation; }
		void setRotation(glm::vec3 rotation) { _Rotation = rotation; recalculateViewMatrix(); }

		glm::vec3 right() const { return _Right; }
		glm::vec3 front() const { return _Front; }
		glm::vec3 up() const { return _Up; }

		const glm::mat4& getProjectionMatrix() const { return _ProjectionMatrix; }
		const glm::mat4& getViewMatrix() const { return _ViewMatrix; }
		const glm::mat4& getViewProjectionMatrix() const { return _ViewProjectionMatrix; }

	private:
		void recalculateViewMatrix();
		bool onWindowResized(WindowResizeEvent& e);

		Projection _projection;

		glm::mat4 _ProjectionMatrix;
		glm::mat4 _ViewMatrix;
		glm::mat4 _ViewProjectionMatrix;

		glm::vec3 _Position = {  0.0f, 0.0f, 0.0f };
		glm::vec3 _Rotation = { 0.0f, 0.0f, 0.0f }; //(roll, pitch, yaw)

		glm::vec3 _Target = { 0.0f, 0.0f, 0.0f };
		glm::vec3 _WorldUp = { 0.0f, 0.0f, 1.0f };

		glm::vec3 _Right = { 1.0f, 0.0f, 0.0f };
		glm::vec3 _Front = { 0.0f, 1.0f, 0.0f };
		glm::vec3 _Up    = { 0.0f, 0.0f, 1.0f };

		int _width, _height;

		float _AspectRatio;
		float _fov;
		float _nearPlane;
		float _farPlane;
		float _zoom;
	};

	typedef Shared<Camera> Camera_Ptr;

}
