#include "app/terrain_scaler.h"

#include <glm/ext/matrix_transform.hpp>

namespace editor
{
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// DistanceMetric
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	DistanceMetric DistanceMetric::FromMeters(float meters)
	{
		return DistanceMetric::FromKilometers(meters * 1e-3f);
	}

	DistanceMetric DistanceMetric::FromKilometers(float kilometers)
	{
		return DistanceMetric(kilometers);
	}

	float DistanceMetric::GetMeters() const
	{
		return mKilometers * 1e-3f;
	}

	float DistanceMetric::GetKilometers() const
	{
		return mKilometers;
	}

	DistanceMetric::DistanceMetric(float kilometers)
		: mKilometers(kilometers)
	{}

	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//
	// TerrainScaler
	//
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	void TerrainScaler::SetScale(DistanceMetric scale)
	{
		mScale = scale;
		RecalculateModelMatrix();
	}

	DistanceMetric TerrainScaler::GetScale() const
	{
		return mScale;
	}

	void TerrainScaler::SetWidth(DistanceMetric width)
	{
		mWidth = width;
		RecalculateModelMatrix();
	}

	DistanceMetric TerrainScaler::GetWidth() const
	{
		return mWidth;
	}

	void TerrainScaler::SetDepth(DistanceMetric depth)
	{
		mDepth = depth;
		RecalculateModelMatrix();
	}

	DistanceMetric TerrainScaler::GetDepth() const
	{
		return mDepth;
	}

	void TerrainScaler::SetMaxHeight(DistanceMetric maxHeight)
	{
		mMaxHeight = maxHeight;
		RecalculateModelMatrix();
	}

	DistanceMetric TerrainScaler::GetMaxHeight() const
	{
		return mMaxHeight;
	}

	const glm::mat4& TerrainScaler::GetModelMatrix() const
	{
		return mModelMatrix;
	}

	void TerrainScaler::RecalculateModelMatrix()
	{
		float scale = mScale.GetMeters();
		float scaleX = scale / mWidth.GetMeters();
		float scaleZ = scale / mDepth.GetMeters();
		float scaleY = scale / mMaxHeight.GetMeters();

		mModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaleX, scaleY, scaleZ));
	}
}


