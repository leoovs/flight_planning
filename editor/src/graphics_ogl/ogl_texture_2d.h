#pragma once

#include <GL/glew.h>

#include "graphics/texture_2d.h"

namespace editor
{
	class OglTexture2D final : public Texture2D
	{
	public:
		OglTexture2D(Texture2DParams params);
		~OglTexture2D() override;
		
		const Texture2DParams& GetParams() const override;

		void SetData(
			const SubresourceRegion& region,
			const void* srcData) override;

		GLuint GetNativeTexture2D() const;

	private:
		void CreateNativeTexture2D();
		void DestroyNativeTexture2D();

		Texture2DParams mParams;
		GLuint mNativeTexture2D = 0;
	};
}
