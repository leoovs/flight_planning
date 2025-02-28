#pragma once

#include "platform/platform_service.h"

namespace editor
{
	class Sdl3PlatformService final : public PlatformService
	{
	public:
		~Sdl3PlatformService() override = default;

		std::string_view GetName() const override;

		// Унаследовано через PlatformService
		void BeginFrame() override;
		void EndFrame() override;
		 
		void PollEvents() override;
	};
}
