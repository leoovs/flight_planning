#pragma once

#include "uavpf/image/tiff_tag_accessor.h"
#include "image/tiff_image_handle.h"

namespace uavpf 
{
	class MatrixTiffTagAccessor final : public TiffTagAccessor
	{
	public:
		MatrixTiffTagAccessor(
			const TiffImageHandle* imageHandle,
			TiffTag tag);

		TiffTag GetTag() const override;

		glm::mat4 AsMatrix() const override;

	private:
		const TiffImageHandle* mImageHandle = nullptr;
		TiffTag mTag = TiffTag::Unknown;
	};
}

