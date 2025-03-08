#pragma once

#include "graphics/vertex_input.h"

#include <GL/glew.h>

namespace editor
{
	class OglVertexInput final : public VertexInput
	{
	public:
		OglVertexInput(VertexInputParams params);
		~OglVertexInput() override;

		const VertexInputParams& GetParams() const override;

		GLuint GetNativeVertexArray() const;

	private:
		void CreateNativeVertexArray();
		void DestroyNativeVertexArray();

		void AssignVertexBuffers();
		void AssignIndexBuffer();
		void AssignVertexAttributes();

		std::array<size_t, GraphicsBufferSlots::cMaxSlots>
		CalculateMaxAttributeAlignmentPerSlot() const;

		static size_t AlignRawByteWidth(size_t byteWidth, size_t alignment);

		VertexInputParams mParams;
		GLuint mNativeVertexArray = 0;
	};
}

