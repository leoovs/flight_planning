#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <uavpf/uavpf.h>

#include "editor/terrain_editor.h"
#include "uavpf/nav/nav_resolution.h"

namespace editor
{
	struct Notam
	{
		glm::ivec2 NavCoord{};
		float RelativeRadius = 0.3f;
	};

	class NavNetwork
	{
	public:
		NavNetwork(const TerrainEditor& terrainEditor);

		void SetResolution(uavpf::experimental::NavResolution resolution);
		uavpf::experimental::NavResolution GetResolution() const;
		void PopulateHeight();

		const uavpf::experimental::NavGrid& GetGrid() const;

		void AddNotam(Notam notam);
		size_t GetNotamCount() const;
		Notam GetNotam(size_t index) const;
		void SetNotam(size_t index, Notam notam);
		void RemoveNotam(size_t index);

	private:
		const TerrainEditor* mTerrainEditor = nullptr;

		uavpf::experimental::NavResolution mResolution;
		uavpf::experimental::NavGrid mGrid;

		std::vector<Notam> mNotams;
	};
}


