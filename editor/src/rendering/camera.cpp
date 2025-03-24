#include "rendering/camera.h"

namespace editor
{
	void Camera::SetProjectionMatrix(const glm::mat4& projectionMatrix)
	{
		mProjectionMatrix = std::move(projectionMatrix);
	}

	const glm::mat4& Camera::GetProjectionMatrix() const
	{
		return mProjectionMatrix;
	}

	void Camera::SetPosition(const glm::vec3& position)
	{
		mPosition = position;
	}

	const glm::vec3& Camera::GetPosition() const
	{
		return mPosition;
	}

	const glm::vec3& Camera::GetFrontVector() const
	{
		return mFrontVector;
	}

	const glm::vec3& Camera::GetRightVector() const
	{
		return mRightVector;
	}

	const glm::vec3& Camera::GetUpVector() const
	{
		return mUpVector;
	}

	const glm::mat4& Camera::GetLookAtMatrix() const
	{
		return mLookAtMatrix;
	}

	glm::mat4 Camera::CalculateViewProjectionMatrix() const
	{
		return mProjectionMatrix * mLookAtMatrix;
	}

	std::pair<float, float> Camera::CalculatePolarAndAzimuth() const
	{
		float x = mFrontVector.x;
		float y = mFrontVector.y;
		float z = mFrontVector.z;

		float polarAngle = glm::acos(y);
		float azimuthAngle = glm::sign(z) * glm::acos(
			x / glm::sqrt(x*x + z*z));

		return std::make_pair(polarAngle, azimuthAngle);
	}

	void Camera::LookAt(const glm::vec3& target)
	{
		glm::vec3 direction = target - mPosition;
		if (glm::zero<float>() == glm::length(direction))
		{
			direction += glm::vec3(0.0f, 0.0f, -0.1f);
		}

		mFrontVector = glm::normalize(direction);
		mRightVector = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
		mUpVector = glm::cross(mRightVector, mFrontVector);

		mLookAtMatrix = glm::lookAt(mPosition, target, mUpVector);
	}

	void Camera::LookAhead(const glm::vec3& offset)
	{
		LookAt(mPosition + offset);
	}

	void Camera::LookAround(float polarAngle, float azimuthAngle)
	{
		LookAhead(glm::vec3(
			glm::sin(polarAngle) * glm::cos(azimuthAngle),
			glm::cos(polarAngle),
			glm::sin(polarAngle) * glm::sin(azimuthAngle)
		));
	}
}

