#pragma once

#include <glm/glm.hpp>

namespace editor
{
	class DistanceMetric
	{
	public:
		static DistanceMetric FromMeters(float meters);
		static DistanceMetric FromKilometers(float kilometers);

		DistanceMetric() = default;

		float GetMeters() const;
		float GetKilometers() const;

	private:
		DistanceMetric(float kilometers);

		float mKilometers = 1.0f;
	};

	class TerrainScaler
	{
	public:
		void SetScale(DistanceMetric scale);
		DistanceMetric GetScale() const;

		void SetWidth(DistanceMetric width);
		DistanceMetric GetWidth() const;

		void SetDepth(DistanceMetric depth);
		DistanceMetric GetDepth() const;

		void SetMaxHeight(DistanceMetric maxHeight);
		DistanceMetric GetMaxHeight() const;

		const glm::mat4& GetModelMatrix() const;

	private:
		void RecalculateModelMatrix();

		DistanceMetric mScale;
		DistanceMetric mWidth;	
		DistanceMetric mDepth;
		DistanceMetric mMaxHeight;
		glm::mat4 mModelMatrix = glm::mat4(1.0f);
	};
}

