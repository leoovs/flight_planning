#pragma once

#include "editor/terrain_editor.h"
#include "uavpf/nav/nav_resolution.h"
#include <cstddef>

#include <array>

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
		PathPlanner(TerrainEditor& terrainEditor);

		uavpf::experimental::NavResolution GetResolution() const;
		void SetResolution(uavpf::experimental::NavResolution resolution);

		glm::ivec2 GetNavCoord(CheckpointKind kind) const;
		void SetNavCoord(CheckpointKind kind, glm::ivec2 navCoord);

		glm::ivec2 NavCoordToHeightMapCoord(glm::ivec2 navCoord) const;

	private:
		void ClampCheckpointNavCoords();

		TerrainEditor* mTerrainEditor = nullptr;
		std::array<glm::ivec2, +CheckpointKind::Count_> mCheckpointNavCoords{};
		uavpf::experimental::NavGrid mNavGrid;
	};
}

