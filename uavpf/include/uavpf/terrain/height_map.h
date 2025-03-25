#pragma once

#include <cassert>
#include <cinttypes>

#include <vector>

namespace uavpf 
{
	class HeightMap
	{
	public:
		template<typename T>
		class RowAccessor
		{
		public:
			RowAccessor(T* heightMatrix, int32_t x, int32_t width, size_t size)
				: mHeightMatrix(heightMatrix)
				, mOffsetX(x)
				, mWidth(width)
				, mSize(size)
			{
			}

			T& operator[](int32_t y) const
			{
				int32_t index = y * mWidth + mOffsetX;
				assert(index < mSize);
				return mHeightMatrix[index];
			}

		private:
			T* mHeightMatrix = nullptr;
			int32_t mOffsetX = 0;
			int32_t mWidth = 0;
			size_t mSize = 0;
		};

		HeightMap() = default;
		HeightMap(int32_t width, int32_t height);

		RowAccessor<float> operator[](int32_t x);
		RowAccessor<const float> operator[](int32_t x) const;

		size_t GetSize() const;
		int32_t GetWidth() const;
		int32_t GetHeight() const;

		void Resize(int32_t width, int32_t height);

	private:
		int32_t mWidth = 0;
		int32_t mHeight = 0;
		std::vector<float> mHeightMatrix;
	};
}

