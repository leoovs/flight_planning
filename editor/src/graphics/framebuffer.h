#pragma once

#include <string>

#include "graphics/subresource.h"

namespace editor
{
	class Texture2D;

	enum class FramebufferAttachment
	{
		Color,
		DepthStencil,
		Count_,
	};

	struct FramebufferParams
	{
		std::string DebugName = "Unnamed Framebuffer";
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual const FramebufferParams& GetParams() const = 0;

		virtual void AttachTexture2D(
			FramebufferAttachment attachment,
			Texture2D* texture2D,
			SubresourceRegion region) = 0;

		virtual bool HasAttachment(FramebufferAttachment attachment) const = 0;

		virtual void Detach(FramebufferAttachment attachment) = 0;
	};
}

