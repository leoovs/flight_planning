#pragma once

#include "editor/nav_network.h"
#include "editor/terrain_editor.h"
#include "uavpf/nav/nav_resolution.h"
#include "uavpf/nav/step_cost.h"
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

	class NotamCost final : public uavpf::experimental::StepCost
	{
	public:
		NotamCost(glm::vec2 navCoord, glm::vec2 ellipse);

		float Evaluate(
			const uavpf::experimental::NavCell& src,
			const uavpf::experimental::NavCell& dst) const override;

	private:
		glm::vec2 mNotamNavCoord;
		glm::vec2 mNotamNavEllipse;
	};

	class PathPlanner
	{
	public:
		using CheckpointNavCoords = std::array<glm::ivec2, +CheckpointKind::Count_>;
		using CheckpointRelativeCoords = std::array<glm::vec2, +CheckpointKind::Count_>;

		PathPlanner(const TerrainEditor& terrainEditor, const NavNetwork& navNetwork);

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
		bool IsBuildingPath() const;
		void BuildPath();
		void EndBuildPath();
		const std::vector<uavpf::experimental::NavCell>& GetPath() const;

		float GetWorldSpaceElevation() const;
		void SetWorldSpaceElevation(float elevation);

	private:
		void ClampCheckpointNavCoords();
		void PopulateNotamCosts(uavpf::experimental::ComplexCost& costs);

		const TerrainEditor* mTerrainEditor = nullptr;
		const NavNetwork* mNavNetwork = nullptr;
		CheckpointNavCoords mCheckpointNavCoords{};
		std::vector<uavpf::experimental::NavCell> mNavPath;
		std::atomic_bool mBuildingPath;
		float mWorldSpaceElevation = 0.02f;
	};
}

