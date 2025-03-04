#pragma once

namespace editor
{
	class OglProvider
	{
	public:
		virtual ~OglProvider() = default;

		virtual void SwapBuffers() = 0;
	};
}

