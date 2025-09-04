#pragma once

#include <filesystem>

#include <glm/glm.hpp>

#include "editor/nav_network.h"
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
		float WorldScale = 1.0f;
		float HeightScale = 1.0f;

		UpdateTerrainScaleEvent(float world, float height)
			: WorldScale(world)
			, HeightScale(height)
		{}
	};

	class UpdateNavGridResolutionEvent final : public Event
	{
	public:
		uavpf::NavResolution Resolution;

		UpdateNavGridResolutionEvent(uavpf::NavResolution res)
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

	class UpdatePathPlannerWeightEvent final : public Event
	{
	public:
		PathPlannerWeight Name = PathPlannerWeight::Distance;
		float Weight = 0.0f;

		UpdatePathPlannerWeightEvent(PathPlannerWeight name, float weight)
			: Name(name)
			, Weight(weight)
		{}
	};

	class BuildPathEvent final : public Event
	{};

	class CancelBuildPathEvent final : public Event
	{};

	class PathBuiltEvent final : public Event
	{};

	class AddNotamEvent final : public Event
	{};

	class UpdateNotamEvent final : public Event
	{
	public:
		Notam UpdatedNotam;
		size_t UpdatedNotamIndex = 0;

		UpdateNotamEvent(Notam updatedNotam, size_t index)
			: UpdatedNotam(updatedNotam)
			, UpdatedNotamIndex(index)
		{}
	};

	class RemoveNotamEvent final : public Event
	{
	public:
		int32_t NotamIndex = 0;

		RemoveNotamEvent(int32_t notamIndex)
			: NotamIndex(notamIndex)
		{}
	};

	class ClearNotamsEvent final : public Event
	{};
}

