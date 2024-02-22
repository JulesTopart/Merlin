#pragma once
#include "Merlin/Core/Core.h"

#include "Merlin/Graphics/Shader.h"
#include "Merlin/Events/ApplicationEvent.h"

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
		void OnEvent(Event& e);

		void Translate(float dx, float dy, float dz);//3D
		void Translate(float dx, float dy);//2D
		void Translate(glm::vec3 du3d);
		void Translate(glm::vec2 du2d);

		void Rotate(float dRx, float dRy, float dRz);
		void Rotate(glm::vec3 dR);

		bool isOrthoGraphic() const { return _projection == Projection::Orthographic; }
		//bool () const { return _projection == Projection::Orthographic; }

		void Reset();
		void ResetProjection();

		inline void setZoom(float zoom) { _zoom = zoom; ResetProjection();}
		inline void setFOV(float fov) { _fov = fov; ResetProjection(); }
		inline void setNearPlane(float np) { _nearPlane = np; ResetProjection();}
		inline void setFarPlane(float fp) { _farPlane = fp; ResetProjection();}

		inline const float GetZoom() const { return _zoom; }
		inline const float GetAspectRatio() const { return _AspectRatio; }
		inline const int Width() const { return _width; }
		inline const int Height() const { return _height; }

		const glm::vec3& GetPosition() const { return _Position; }
		void SetPosition(const glm::vec3& position) { _Position = position; RecalculateViewMatrix(); }
		const glm::vec3& GetTarget() const { return _Target; }
		void SetTarget(const glm::vec3& target) { _Target = target; RecalculateViewMatrix(); }
		glm::vec3 GetRotation() const { return _Rotation; }
		void SetRotation(glm::vec3 rotation) { _Rotation = rotation; RecalculateViewMatrix(); }

		glm::vec3 Right() const { return _Right; }
		glm::vec3 Front() const { return _Front; }
		glm::vec3 Up() const { return _Up; }

		const glm::mat4& GetProjectionMatrix() const { return _ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return _ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return _ViewProjectionMatrix; }

	private:
		void RecalculateViewMatrix();
		bool OnWindowResized(WindowResizeEvent& e);

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
