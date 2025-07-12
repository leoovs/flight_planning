#include "uavpf/image/tiff_loader.h"

#include <filesystem>

#include "image/tiff_image_handle.h"

namespace uavpf
{
	TiffImage TiffLoader::LoadImageFromFile(std::filesystem::path imagePath)
	{
		if (!std::filesystem::exists(imagePath))
		{
			mLastLoadStatus = TiffLoadStatus::FileNotFound;
			return TiffImage();
		}

		TIFF* nativeTiff = nullptr;

		// Ugly workaround for different fs::path implementations
#ifdef _WIN32
			nativeTiff = TIFFOpenW(imagePath.wstring().c_str(), "r");
#else
			nativeTiff = TIFFOpen(imagePath.u8string().c_str(), "r");
#endif

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

