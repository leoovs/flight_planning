#include "graphics_ogl/ogl_texture_2d.h"

namespace editor
{
	OglTexture2D::OglTexture2D(Texture2DParams params)
		: mParams(std::move(params))
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &mTextureName);
	}

	OglTexture2D::~OglTexture2D()
	{
		glDeleteTextures(1, &mTextureName);
	}

	const Texture2DParams& OglTexture2D::GetParams() const
	{
		return mParams;
	}

	void OglTexture2D::SetData(const void* data)
	{

	}
}
