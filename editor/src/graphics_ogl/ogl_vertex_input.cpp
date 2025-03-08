#include "graphics_ogl/ogl_vertex_input.h"

#include <cstddef>
#include <uavpf/uavpf.h>

#include "graphics/graphics_format.h"
#include "graphics_ogl/ogl_facts.h"
#include "graphics_ogl/ogl_graphics_buffer.h"

namespace editor
{
	OglVertexInput::OglVertexInput(VertexInputParams params)
		: mParams(std::move(params))
	{
		CreateNativeVertexArray();

		AssignVertexBuffers();
		AssignIndexBuffer();
		AssignVertexAttributes();
	}

	OglVertexInput::~OglVertexInput()
	{
		DestroyNativeVertexArray();
	}

	const VertexInputParams& OglVertexInput::GetParams() const
	{
		return mParams;
	}

	GLuint OglVertexInput::GetNativeVertexArray() const
	{
		return mNativeVertexArray;
	}

	void OglVertexInput::CreateNativeVertexArray()
	{
		glCreateVertexArrays(1, &mNativeVertexArray);
		glObjectLabel(
			GL_VERTEX_ARRAY,
			mNativeVertexArray,
			mParams.DebugName.length(),
			mParams.DebugName.data());
	}

	void OglVertexInput::DestroyNativeVertexArray()
	{
		glDeleteVertexArrays(1, &mNativeVertexArray);
		mNativeVertexArray = 0;
	}

	void OglVertexInput::AssignVertexBuffers()
	{
		for (GraphicsBufferSlots::Iterator it = mParams.VertexBuffers.begin();
			it != mParams.VertexBuffers.end();
			it++)
		{
			// Slot for vertex buffer is empty, move on to the next one.
			if (nullptr == *it)
			{
				continue;
			}

			size_t slotIndex = mParams.VertexBuffers.ConvertIteratorToSlotIndex(it);
			auto* vertexBuffer = dynamic_cast<OglGraphicsBuffer*>(*it);

			GraphicsBufferTarget target = vertexBuffer->GetParams().Target;
			if (GraphicsBufferTarget::Vertex != target)
			{
				UAVPF_LOG(
					Application,
					Error,
					"Expected Vertex buffer for vertex input, got something"
					" else");
				continue;
			}

			GLuint nativeBuffer = vertexBuffer->GetNativeBuffer();
			auto stride = static_cast<GLsizei>(vertexBuffer->GetParams().StructSize);

			glVertexArrayVertexBuffer(
				mNativeVertexArray,
				slotIndex,
				nativeBuffer,
				0,
				stride);
		}
	}

	void OglVertexInput::AssignIndexBuffer()
	{
		if (nullptr == mParams.IndexBuffer)
		{
			return;
		}

		auto* indexBuffer = dynamic_cast<OglGraphicsBuffer*>(mParams.IndexBuffer);

		GraphicsBufferTarget target = indexBuffer->GetParams().Target;
		if (GraphicsBufferTarget::Index != target)
		{
				UAVPF_LOG(
					Application,
					Error,
					"Expected Index buffer for vertex input, got something"
					" else");
			return;
		}

		GLuint nativeBuffer = indexBuffer->GetNativeBuffer();

		glVertexArrayElementBuffer(mNativeVertexArray, nativeBuffer);
	}

	void OglVertexInput::AssignVertexAttributes()
	{
		const std::array<size_t, GraphicsBufferSlots::cMaxSlots>
		cMaxAlignmentPerSlot = CalculateMaxAttributeAlignmentPerSlot();

		std::array<size_t, GraphicsBufferSlots::cMaxSlots> byteOffsetPerSlot = {};

		for (auto it = mParams.VertexAttributes.begin();
			it != mParams.VertexAttributes.end();
			it++)
		{
			size_t slotIndex = it->VertexBufferSlotIndex;
			GraphicsFormat attributeFormat = it->Format;
			ptrdiff_t attributeIndex = std::distance(
				mParams.VertexAttributes.begin(),
				it);

			size_t& slotByteOffset = byteOffsetPerSlot.at(slotIndex);
			size_t byteOffset = 0;

			if (VertexAttributeParams::cAppendAlignedByteOffset == it->ByteOffset)
			{
				byteOffset = slotByteOffset;
			}
			else
			{
				byteOffset = it->ByteOffset;
				slotByteOffset = byteOffset;
			}

			const size_t cMaxSlotAlignment = cMaxAlignmentPerSlot.at(slotIndex);	

			size_t elementCount = GetGraphicsFormatElementCount(attributeFormat);
			GLenum nativeType = OglFacts::ConvertGraphicsFormatToType(attributeFormat);

			glEnableVertexArrayAttrib(
				mNativeVertexArray,
				static_cast<GLuint>(attributeIndex));
			glVertexArrayAttribBinding(
				mNativeVertexArray,
				static_cast<GLuint>(attributeIndex),
				static_cast<GLuint>(slotIndex));
			glVertexArrayAttribFormat(
				mNativeVertexArray,
				static_cast<GLuint>(attributeIndex),
				static_cast<GLint>(elementCount),
				nativeType,
				OglFacts::IsGraphicsFormatNormalized(attributeFormat),
				static_cast<GLuint>(byteOffset));

			size_t alignedAttributeByteWidth = AlignRawByteWidth(
				GetGraphicsFormatByteWidth(attributeFormat),
				cMaxSlotAlignment);

			slotByteOffset += alignedAttributeByteWidth;
		}
	}

	std::array<size_t, GraphicsBufferSlots::cMaxSlots>
	OglVertexInput::CalculateMaxAttributeAlignmentPerSlot() const
	{
		std::array<size_t, GraphicsBufferSlots::cMaxSlots> maxAlignmentPerSlot = {};

		for (auto it = mParams.VertexAttributes.begin();
			it != mParams.VertexAttributes.end();
			it++)
		{
			size_t slotIndex = it->VertexBufferSlotIndex;
			GraphicsFormat attributeFormat = it->Format;

			size_t& maxAlignment = maxAlignmentPerSlot.at(slotIndex);
			size_t attributeAlignment = GetGraphicsFormatAlignment(attributeFormat);

			maxAlignment = std::max(maxAlignment, attributeAlignment);
		}

		return maxAlignmentPerSlot;
	}

	size_t OglVertexInput::AlignRawByteWidth(size_t byteWidth, size_t alignment)
	{
		if (byteWidth % alignment == 0)
		{
			return byteWidth;
		}

		size_t alignmentAddup = alignment - byteWidth % alignment;
		return byteWidth + alignmentAddup;
	}
}


