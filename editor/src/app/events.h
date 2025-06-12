#pragma once

#include "event/event.h"

namespace editor
{
	class CancelPathFindingEvent final : public Event
	{};	

	class PathFoundEvent final : public Event
	{};
}

