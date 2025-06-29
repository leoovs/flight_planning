#pragma once

#include <filesystem>

#include <glm/glm.hpp>

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

	class UpdateTerrainScaleEvent final : public Event
	{
	public:
		glm::vec3 TerrainScale;

		UpdateTerrainScaleEvent(glm::vec3 terrainScale)
			: TerrainScale(terrainScale)
		{}
	};
}

