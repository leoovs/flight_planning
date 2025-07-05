#include "editor/nav_network.h"

namespace editor
{
	NavNetwork::NavNetwork(const TerrainEditor& terrainEditor)
		: mTerrainEditor(&terrainEditor)
	{}

	void NavNetwork::SetResolution(uavpf::experimental::NavResolution resolution)
	{
		mResolution = resolution;
	}

	uavpf::experimental::NavResolution NavNetwork::GetResolution() const
	{
		return mResolution;
	}

	void NavNetwork::PopulateHeight()
	{
		const uavpf::HeightMap& hm = mTerrainEditor->GetHeightMap();

		mGrid = uavpf::experimental::NavGridBuilder()
			.Resize(mResolution.Width, mResolution.Depth)
			.PopulateHeight(hm)
			.Build();
	}

	const uavpf::experimental::NavGrid& NavNetwork::GetGrid() const
	{
		return mGrid;
	}

	void NavNetwork::AddNotam(Notam notam)
	{
		mNotams.push_back(notam);
	}

	size_t NavNetwork::GetNotamCount() const
	{
		return mNotams.size();
	}

	Notam NavNetwork::GetNotam(size_t index) const
	{
		return mNotams.at(index);
	}

	void NavNetwork::SetNotam(size_t index, Notam notam)
	{
		mNotams.at(index) = notam;
	}
}

