#pragma once

#include "editor/terrain_editor.h"
#include "uavpf/nav/nav_resolution.h"
#include <cstddef>

#include <array>
#include <atomic>

#include <glm/glm.hpp>
#include <uavpf/nav/nav_grid.h>

namespace editor
{
	enum class CheckpointKind
	{
		Start,
		End,
		Count_,
	};

	constexpr size_t operator+(CheckpointKind kind)
	{
		return static_cast<size_t>(kind);
	}

	class PathPlanner
	{
	public:
		using CheckpointNavCoords = std::array<glm::ivec2, +CheckpointKind::Count_>;
		using CheckpointRelativeCoords = std::array<glm::vec2, +CheckpointKind::Count_>;

		PathPlanner(TerrainEditor& terrainEditor);

		uavpf::experimental::NavResolution GetResolution() const;
		void SetResolution(uavpf::experimental::NavResolution resolution);

		glm::ivec2 GetNavCoord(CheckpointKind kind) const;
		void SetNavCoord(CheckpointKind kind, glm::ivec2 navCoord);

		glm::vec2 GetRelativeCoord(CheckpointKind kind) const;
		void SetRelativeCoord(CheckpointKind kind, glm::vec2 relCoord);

		CheckpointNavCoords QueryCheckpointNavCoords() const;
		CheckpointRelativeCoords QueryCheckpointRelativeCoords() const;

		void SetCheckpointNavCoords(const CheckpointNavCoords& navCoords);
		void SetCheckpointRelativeCoords(const CheckpointRelativeCoords& relativeCoords);

		glm::ivec2 NavCoordToHeightMapCoord(glm::ivec2 navCoord) const;

		void BeginBuildPath();
		bool IsBuildingPath();
		void BuildPath();
		void EndBuildPath();
		const std::vector<uavpf::experimental::NavCell>& GetPath() const;

		float GetWorldSpaceElevation() const;
		void SetWorldSpaceElevation(float elevation);

		// TODO: should we add height map space elevation instead?

	private:
		void ClampCheckpointNavCoords();

		TerrainEditor* mTerrainEditor = nullptr;
		CheckpointNavCoords mCheckpointNavCoords{};
		uavpf::experimental::NavGrid mNavGrid;
		std::vector<uavpf::experimental::NavCell> mNavPath;
		std::atomic_bool mBuildingPath;
		float mWorldSpaceElevation = 0.02f;
	};
}

