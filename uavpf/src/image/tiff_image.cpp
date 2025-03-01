#include "uavpf/image/tiff_image.h"

#include <cassert>

#include "image/tiff_image_handle.h"

namespace uavpf
{
	TiffImage::TiffImage() = default;

	TiffImage::TiffImage(std::unique_ptr<TiffImageHandle> imageHandle)
		: mImageHandle(std::move(imageHandle))
	{
	}

	TiffImage::TiffImage(TiffImage&& other) noexcept = default;

	TiffImage& TiffImage::operator=(TiffImage&& other) noexcept = default;

	TiffImage::~TiffImage() = default;

	int32_t TiffImage::GetWidth() const
	{
		return mImageHandle->Width;
	}

	int32_t TiffImage::GetHeight() const
	{
		return mImageHandle->Height;
	}

	uint32_t TiffImage::ReadPixelAt(int32_t x, int32_t y) const
	{
		size_t indexFrom2DCoords = y * mImageHandle->Width + x;

		assert(indexFrom2DCoords < mImageHandle->RgbaPixels.size());

		return mImageHandle->RgbaPixels.at(indexFrom2DCoords);
	}

	bool TiffImage::IsValidImage() const
	{
		return nullptr != mImageHandle;
	}

	TiffImage::operator bool() const
	{
		return IsValidImage();
	}
}
