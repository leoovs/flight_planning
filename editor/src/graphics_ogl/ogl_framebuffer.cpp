#include "graphics_ogl/ogl_framebuffer.h"

#include "graphics_ogl/ogl_facts.h"
#include "graphics_ogl/ogl_texture_2d.h"

namespace editor
{
	OglFramebuffer::OglFramebuffer(FramebufferParams params)
		: mParams(std::move(params))
	{
		CreateNativeFramebuffer();
	}

	OglFramebuffer::~OglFramebuffer()
	{
		DestroyNativeFramebuffer();
	}

	const FramebufferParams& OglFramebuffer::GetParams() const
	{
		return mParams;
	}

	void OglFramebuffer::AttachTexture2D(
		FramebufferAttachment attachment,
		Texture2D* texture2D,
		SubresourceRegion region)
	{
		auto actualTexture2D = dynamic_cast<OglTexture2D*>(texture2D);
		GLenum nativeAttachment = OglFacts::ConvertFramebufferAttachmentToNative(attachment);

		glNamedFramebufferTexture(
			mNativeFramebuffer,
			nativeAttachment,
			actualTexture2D->GetNativeTexture2D(),
			region.MipLevelIndex);
	}

	void OglFramebuffer::Detach(FramebufferAttachment attachment)
	{
		GLenum nativeAttachment = OglFacts::ConvertFramebufferAttachmentToNative(attachment);

		glNamedFramebufferTextureLayer(
			mNativeFramebuffer,
			nativeAttachment,
			0,
			0,
			0);
	}

	GLuint OglFramebuffer::GetNativeFramebuffer() const
	{
		return mNativeFramebuffer;
	}

	void OglFramebuffer::CreateNativeFramebuffer()
	{
		glCreateFramebuffers(1, &mNativeFramebuffer);

		glObjectLabel(
			GL_FRAMEBUFFER,
			mNativeFramebuffer,
			mParams.DebugName.length(),
			mParams.DebugName.data());

		GLenum drawBuffer[]{ GL_COLOR_ATTACHMENT0 };
		glNamedFramebufferDrawBuffers(
			mNativeFramebuffer,
			std::size(drawBuffer),
			drawBuffer);
	}

	void OglFramebuffer::DestroyNativeFramebuffer()
	{
		glDeleteFramebuffers(1, &mNativeFramebuffer);
		mNativeFramebuffer = 0;
	}
}
