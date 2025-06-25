#pragma once

#include <array>

namespace uavpf::experimental
{
	enum class ExplorationDirection
	{
		None,

		North,
		South,
		East,
		West,
		
		NorthWest,
		NorthEast,
		SouthEast,
		SouthWest,
	};

	constexpr std::array<ExplorationDirection, 8> EnumerateExplorationDirections()
	{
		return
		{
			ExplorationDirection::North,
			ExplorationDirection::South,
			ExplorationDirection::East,
			ExplorationDirection::West,

			ExplorationDirection::NorthWest,
			ExplorationDirection::NorthEast,
			ExplorationDirection::SouthEast,
			ExplorationDirection::SouthWest,
		};
	}
}

