#pragma once

#include <cassert>
#include <cinttypes>

#include <vector>

namespace uavpf 
{
	class ImageGrayscale;

	class HeightMap
	{
	public:
		template<typename T>
		class RowAccessor
		{
		public:
			RowAccessor(T* heightMatrix, int32_t row, int32_t width, size_t size)
				: mHeightMatrix(heightMatrix)
				, mRow(row)
				, mWidth(width)
				, mSize(size)
			{
			}

			T& operator[](int32_t column) const
			{
				int32_t index = mRow * mWidth + column;
				assert(index < mSize);
				return mHeightMatrix[index];
			}

		private:
			T* mHeightMatrix = nullptr;
			int32_t mRow = 0;
			int32_t mWidth = 0;
			size_t mSize = 0;
		};

		HeightMap() = default;
		HeightMap(int32_t width, int32_t height);
		HeightMap(int32_t width, int32_t height, std::vector<float> heightMatrix);

		RowAccessor<float> operator[](int32_t row);
		RowAccessor<const float> operator[](int32_t row) const;

		size_t GetSize() const;
		int32_t GetWidth() const;
		int32_t GetHeight() const;

		void Resize(int32_t width, int32_t height);

		std::vector<float>::iterator begin();
		std::vector<float>::iterator end();

	private:
		int32_t mWidth = 0;
		int32_t mHeight = 0;
		std::vector<float> mHeightMatrix;
	};
}

