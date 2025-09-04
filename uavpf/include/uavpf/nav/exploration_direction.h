#pragma once

#include <array>

#include <glm/glm.hpp>

namespace uavpf
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

	constexpr glm::ivec2 ExplorationDirectionToCoordOffset(ExplorationDirection dir)
	{
		switch (dir)
		{
			case ExplorationDirection::None:
				return { 0, 0 };
			case ExplorationDirection::North:
				return {  0,  1 };
			case ExplorationDirection::South:
				return {  0, -1 };
			case ExplorationDirection::East:
				return {  1,  0 };
			case ExplorationDirection::West:
				return { -1,  0 };
			case ExplorationDirection::NorthWest:
				return { -1,  1 };
			case ExplorationDirection::NorthEast:
				return {  1,  1 };
			case ExplorationDirection::SouthEast:
				return {  1, -1 };
			case ExplorationDirection::SouthWest:
				return { -1, -1 };
			default:
				return {};
		}
	}
}

