#include "uavpf/image/tiff_loader.h"

#include <filesystem>

#include "image/tiff_image_handle.h"

namespace uavpf
{
	TiffImage TiffLoader::LoadImageFromFile(std::string_view fileName)
	{
		if (!std::filesystem::exists(fileName))
		{
			mLastLoadStatus = TiffLoadStatus::FileNotFound;
			return TiffImage();
		}

		TIFF* nativeTiff = TIFFOpen(fileName.data(), "r");
		if (nullptr == nativeTiff)
		{
			mLastLoadStatus = TiffLoadStatus::LoaderFailure;
			return TiffImage();
		}

		auto imageHandle = std::make_unique<TiffImageHandle>();
		imageHandle->NativeTiff = nativeTiff;

		if (!ReadImageSize(imageHandle.get()))
		{
			return TiffImage();
		}
		if (!ReadImagePixels(imageHandle.get()))
		{
			return TiffImage();
		}

		mLastLoadStatus = TiffLoadStatus::Success;
		return TiffImage(std::move(imageHandle));
	}

	TiffLoadStatus TiffLoader::GetLoadStatus() const
	{
		return mLastLoadStatus;
	}

	bool TiffLoader::ReadImageSize(TiffImageHandle* handle)
	{
		if (nullptr == handle)
		{
			return false;
		}

		uint32_t width = 0;
		uint32_t height = 0;
		if (!TIFFGetField(handle->NativeTiff, TIFFTAG_IMAGEWIDTH, &width))
		{
			mLastLoadStatus = TiffLoadStatus::SizeReadFailure;
			return false;
		}

		if (!TIFFGetField(handle->NativeTiff, TIFFTAG_IMAGELENGTH, &height))
		{
			mLastLoadStatus = TiffLoadStatus::SizeReadFailure;
			return false;
		}

		handle->Width = width;
		handle->Height = height;

		return true;
	}

	bool TiffLoader::ReadImagePixels(TiffImageHandle* handle)
	{
		if (nullptr == handle)
		{
			return false;
		}

		handle->RgbaPixels.resize(handle->Width * handle->Height);

		int result = TIFFReadRGBAImage(
			handle->NativeTiff,
			handle->Width,
			handle->Height,
			handle->RgbaPixels.data());

		if (0 == result)
		{
			mLastLoadStatus = TiffLoadStatus::PixelsReadFailure;
			return false;
		}

		return true;
	}
}

