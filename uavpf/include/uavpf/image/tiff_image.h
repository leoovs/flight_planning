#pragma once

#include <cinttypes>

#include <memory>

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

	private:
		std::unique_ptr<TiffImageHandle> mImageHandle;
	};
}

