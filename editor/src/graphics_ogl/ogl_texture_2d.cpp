#include "graphics_ogl/ogl_texture_2d.h"

#include "graphics_ogl/ogl_facts.h"

namespace editor
{
	OglTexture2D::OglTexture2D(Texture2DParams params)
		: mParams(std::move(params))
	{
		CreateNativeTexture2D();
	}

	OglTexture2D::~OglTexture2D()
	{
		DestroyNativeTexture2D();
	}

	const Texture2DParams& OglTexture2D::GetParams() const
	{
		return mParams;
	}

	void OglTexture2D::SetData(
		const SubresourceRegion& region,
		const void* srcData)
	{
		glTextureSubImage2D(
			mNativeTexture2D,
			region.MipLevelIndex,
			region.OffsetX,
			region.OffsetY,
			region.Width,
			region.Height,
			OglFacts::ConvertGraphicsFormatToFormat(mParams.Format),
			OglFacts::ConvertGraphicsFormatToType(mParams.Format),
			srcData);
	}

	GLuint OglTexture2D::GetNativeTexture2D() const
	{
		return mNativeTexture2D;
	}

	void OglTexture2D::CreateNativeTexture2D()
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &mNativeTexture2D);
		glTextureStorage2D(
			mNativeTexture2D,
			mParams.MipLevelCount,
			OglFacts::ConvertGraphicsFormatToInternalFormat(mParams.Format),
			mParams.Width,
			mParams.Height);
		glObjectLabel(
			GL_TEXTURE,
			mNativeTexture2D,
			static_cast<GLsizei>(mParams.DebugName.length()),
			mParams.DebugName.data());
	}

	void OglTexture2D::DestroyNativeTexture2D()
	{
		glDeleteTextures(1, &mNativeTexture2D);
	}
}
