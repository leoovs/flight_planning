#include "uavpf/nav/path_graph.h"

namespace uavpf::experimental
{
	PathNode& PathGraph::FindForCell(const NavCell& cell)
	{
		return mNodes[cell.Index];
	}
}

