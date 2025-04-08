#pragma once

#include <array>

#include <GL/glew.h>

#include "graphics/framebuffer.h"
#include "graphics_ogl/ogl_texture_2d.h"

namespace editor
{
	class OglFramebuffer final : public Framebuffer
	{
	public:
		OglFramebuffer(FramebufferParams params);
		~OglFramebuffer() override;

		const FramebufferParams& GetParams() const override;

		void AttachTexture2D(
			FramebufferAttachment attachment,
			Texture2D* texture2D,
			SubresourceRegion region) override;

		bool HasAttachment(FramebufferAttachment attachment) const override;

		void Detach(FramebufferAttachment attachment) override;

		GLuint GetNativeFramebuffer() const;

	private:
		void CreateNativeFramebuffer();
		void DestroyNativeFramebuffer();

		FramebufferParams mParams;
		GLuint mNativeFramebuffer = 0;
		std::array<OglTexture2D*, size_t(FramebufferAttachment::Count_)> mAttachments;
	};
}

