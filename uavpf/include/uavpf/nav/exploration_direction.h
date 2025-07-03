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

	constexpr auto EnumerateExplorationDirections()
	{
		constexpr std::array enumeration
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

		return enumeration;
	}
}

