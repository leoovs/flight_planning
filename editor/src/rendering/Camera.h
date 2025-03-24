#pragma once

#include <utility>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace editor
{
	class Camera
	{
	public:
		void SetProjectionMatrix(const glm::mat4& projectionMatrix);
		const glm::mat4& GetProjectionMatrix() const;

		void SetPosition(const glm::vec3& position);
		const glm::vec3& GetPosition() const;

		const glm::vec3& GetFrontVector() const;
		const glm::vec3& GetRightVector() const;
		const glm::vec3& GetUpVector() const;
		const glm::mat4& GetLookAtMatrix() const;

		std::pair<float, float> CalculatePolarAndAzimuth() const;
		glm::mat4 CalculateViewProjectionMatrix() const;

		void LookAt(const glm::vec3& target);
		void LookAhead(const glm::vec3& offset);
		void LookAround(float polarAngle, float azimuthAngle);

	private:
		glm::mat4 mProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 mLookAtMatrix = glm::mat4(1.0f);

		glm::vec3 mPosition = glm::vec3(0.0f);
		glm::vec3 mFrontVector = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 mRightVector = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 mUpVector = glm::vec3(0.0f, 1.0f, 0.0f);
	};
}

