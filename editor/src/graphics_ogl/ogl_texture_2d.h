#pragma once

#include "graphics/texture_2d.h"

namespace editor
{
	class OglTexture2D : public Texture2D {

		OglTexture2D(Texture2DParams* params);
		~OglTexture2D() override;
		
		const Texture2DParams& GetParams() const override;

		void SetData(const void* data) override;

	private:
		Texture2DParams mParams;
	};
}
