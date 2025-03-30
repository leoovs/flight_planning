#include "image/tiff_tag_accessor_impl.h"

#include "uavpf/debug/logger_provider.h"

namespace uavpf
{
	MatrixTiffTagAccessor::MatrixTiffTagAccessor(
		const TiffImageHandle* imageHandle,
		TiffTag tag)
		: mImageHandle(imageHandle)
		, mTag(tag)
	{
	}

	TiffTag MatrixTiffTagAccessor::GetTag() const
	{
		return mTag;
	}

	glm::mat4 MatrixTiffTagAccessor::AsMatrix() const
	{
		double transform[16]{};
		auto nativeTag = static_cast<uint32_t>(mTag);
		int isDefined = TIFFGetField(mImageHandle->NativeTiff.get(), nativeTag, &transform);

		if (0 == isDefined)
		{
			UAVPF_LOG(
				ImageOperation,
				Warning,
				"Tiff tag is not defined");
			return glm::mat4(1.0f);
		};

		return
		{
			transform[0],  transform[1],  transform[2],  transform[3], 
			transform[4],  transform[5],  transform[6],  transform[7], 
			transform[8],  transform[9],  transform[10], transform[11], 
			transform[12], transform[13], transform[14], transform[15], 
		};
	}
}

