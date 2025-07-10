#include "uavpf/image/tiff_loader.h"

#include <filesystem>

#include <unipp/convert.hpp>

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

		std::u16string uniFileName;
		unipp::convert(fileName.begin(), fileName.end(), std::back_inserter(uniFileName));
		std::wstring wideFileName(uniFileName.begin(), uniFileName.end());

		TIFF* nativeTiff = TIFFOpenW(wideFileName.data(), "r");
		if (nullptr == nativeTiff)
		{
			mLastLoadStatus = TiffLoadStatus::LoaderFailure;
			return TiffImage();
		}

		auto imageHandle = std::make_unique<TiffImageHandle>();
		imageHandle->NativeTiff = TiffUniquePtr(nativeTiff);

		if (!ReadImageSize(imageHandle.get()))
		{
			mLastLoadStatus = TiffLoadStatus::SizeReadFailure;
			return TiffImage();
		}
		if (!ReadImagePixels(imageHandle.get()))
		{
			mLastLoadStatus = TiffLoadStatus::PixelsReadFailure;
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
		if (!TIFFGetField(handle->NativeTiff.get(), TIFFTAG_IMAGEWIDTH, &width))
		{
			return false;
		}

		if (!TIFFGetField(handle->NativeTiff.get(), TIFFTAG_IMAGELENGTH, &height))
		{
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
			handle->NativeTiff.get(),
			handle->Width,
			handle->Height,
			handle->RgbaPixels.data());

		if (0 == result)
		{
			return false;
		}

		return true;
	}
}

