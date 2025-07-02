#pragma once

#include <filesystem>

#include <glm/glm.hpp>

#include "editor/path_planner.h"
#include "event/event.h"
#include "uavpf/nav/nav_resolution.h"

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

	class CloseHeightMapRequestEvent final : public Event
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

	class UpdateNavGridResolutionEvent final : public Event
	{
	public:
		uavpf::experimental::NavResolution Resolution;

		UpdateNavGridResolutionEvent(uavpf::experimental::NavResolution res)
			: Resolution(res)
		{}
	};

	class UpdateCheckpointNavCoordEvent final : public Event
	{
	public:
		glm::ivec2 NavCoord;
		CheckpointKind Checkpoint;

		UpdateCheckpointNavCoordEvent(glm::ivec2 navCoord, CheckpointKind checkpoint)
			: NavCoord(navCoord)
			, Checkpoint(checkpoint)
		{}
	};

	class BuildPathEvent final : public Event
	{};

	class CancelBuildPathEvent final : public Event
	{};

	class PathBuiltEvent final : public Event
	{};
}

