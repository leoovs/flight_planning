#include "uavpf/nav/path_graph.h"

namespace uavpf
{
	PathNode& PathGraph::FindForCell(const NavCell& cell)
	{
		return mNodes[cell.Index];
	}
}

