#pragma once

#include "graphics/graphics_buffer.h"

#include <GL/glew.h>

namespace editor
{
	class OglGraphicsBuffer final : public GraphicsBuffer
	{
	public:
		OglGraphicsBuffer(GraphicsBufferParams params);
		~OglGraphicsBuffer() override;

		const GraphicsBufferParams& GetParams() const override;

		void SetData(const void* data, size_t sizeOfData) override;

		GLuint GetNativeBuffer() const;

	private:
		void CreateNativeBuffer();
		void DestroyNativeBuffer();

		GraphicsBufferParams mParams;
		GLuint mNativeBuffer = 0;
	};
}

