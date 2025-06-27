#pragma once

#include <filesystem>

#include "event/event.h"

namespace editor
{
	class HeightMapRequestedEvent final : public Event 
	{
	public:
		std::filesystem::path HeightMapPath;

		HeightMapRequestedEvent(std::filesystem::path heightMapPath)
			: HeightMapPath(std::move(heightMapPath))
		{}
	};

	class HeightMapLoadedEvent final : public Event
	{
	};
}

