#pragma once

#include <glm/glm.hpp>
#include "../Renderer/Shader.h"

namespace Merlin::Scene {

	class Camera
	{
	public:
		Camera();

		//Set projection
		void SetOrthographic(float aspectRatio = 16.0f / 9.0f, float nearPlane = 0.1f, float farPlane = 100.f);
		void SetPerspective(float fov = 45.0f, float aspectRatio = 16.0f / 9.0f, float nearPlane = 0.001f, float farPlane = 100.f);

		void Translate(float dx, float dy, float dz);
		void Translate(glm::vec3 du);

		void Rotate(float dRx, float dRy, float dRz);
		void Rotate(glm::vec3 dR);

		bool isOrthoGraphic() const { return _OrthoGraphic; }

		void Reset();

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


		bool _OrthoGraphic = false;
	};

}
