#include "graphics_ogl/ogl_graphics_buffer.h"

#include "graphics_ogl/ogl_facts.h"

namespace editor
{
	OglGraphicsBuffer::OglGraphicsBuffer(GraphicsBufferParams params)
		: mParams(std::move(params))
	{
		CreateNativeBuffer();
	}

	OglGraphicsBuffer::~OglGraphicsBuffer()
	{
		DestroyNativeBuffer();
	}

	const GraphicsBufferParams& OglGraphicsBuffer::GetParams() const
	{
		return mParams;
	}

	void OglGraphicsBuffer::SetData(const void* data, size_t sizeOfData)
	{
		glNamedBufferSubData(
			mNativeBuffer,
			0,
			static_cast<GLsizeiptr>(sizeOfData),
			data);
	}

	GLuint OglGraphicsBuffer::GetNativeBuffer() const
	{
		return mNativeBuffer;
	}

	void OglGraphicsBuffer::CreateNativeBuffer()
	{
		glCreateBuffers(1, &mNativeBuffer);
		glNamedBufferStorage(
			mNativeBuffer,
			static_cast<GLsizeiptr>(mParams.StructSize * mParams.StructCount),
			nullptr,
			GL_DYNAMIC_STORAGE_BIT);

		glObjectLabel(
			GL_BUFFER,
			mNativeBuffer,
			mParams.DebugName.length(),
			mParams.DebugName.data()); 
	}

	void OglGraphicsBuffer::DestroyNativeBuffer()
	{
		glDeleteBuffers(1, &mNativeBuffer);
		mNativeBuffer = 0;
	}
}

