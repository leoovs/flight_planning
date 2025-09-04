#pragma once

#include <cstddef>

#include <unordered_map>

#include "uavpf/nav/nav_cell.h"
#include "uavpf/nav/path_node.h"

namespace uavpf
{
	class PathGraph
	{
	public:
		PathNode& FindForCell(const NavCell& cell);

	private:
		std::unordered_map<size_t, PathNode> mNodes;
	};
}

