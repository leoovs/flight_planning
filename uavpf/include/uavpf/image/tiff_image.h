#pragma once

#include <cinttypes>

#include <memory>

#include "uavpf/image/tiff_tag_accessor.h"

namespace uavpf
{
	struct TiffImageHandle;

	class TiffImage
	{
	public:
		TiffImage();
		TiffImage(std::unique_ptr<TiffImageHandle> imageHandle);
		TiffImage(TiffImage&& other) noexcept;

		TiffImage& operator=(TiffImage&& other) noexcept;

		~TiffImage();

		int32_t GetWidth() const;
		int32_t GetHeight() const;

		uint32_t ReadPixelAt(int32_t x, int32_t y) const;
		const uint32_t* GetPixels() const;

		std::unique_ptr<TiffTagAccessor> GetTag(TiffTag tag);

		bool IsValidImage() const; 
		explicit operator bool() const;

	private:
		std::unique_ptr<TiffImageHandle> mImageHandle;
	};
}

