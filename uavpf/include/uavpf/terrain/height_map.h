#pragma once

#include <cassert>
#include <cinttypes>
#include <cstddef>

#include <vector>

namespace uavpf 
{
	class ImageGrayscale;

	class HeightMap
	{
	public:
		HeightMap() = default;
		HeightMap(int32_t width, int32_t depth);
		HeightMap(int32_t width, int32_t depth, std::vector<float> heightMatrix);

		size_t GetSize() const;
		int32_t GetWidth() const;
		int32_t GetDepth() const;

		float GetElevation(int32_t x, int32_t z) const;
		float& At(int32_t x, int32_t z);

		void Resize(int32_t width, int32_t depth);

		std::vector<float>::iterator begin();
		std::vector<float>::iterator end();

	private:
		int32_t mWidth = 0;
		int32_t mDepth = 0;
		std::vector<float> mElevation;
	};
}

