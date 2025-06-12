#pragma once

#include <filesystem>

#include "app/asset_storage.h"
#include "event/event.h"

namespace editor
{
	class App;

	class AppQuitEvent final : public Event
	{
	};

	class TiffMapRequestedEvent final : public Event
	{
	public:
		std::filesystem::path TiffMapPath;

		TiffMapRequestedEvent(std::filesystem::path tiffMapPath)
			: TiffMapPath(std::move(tiffMapPath))
		{}
	};

	class TiffMapLoadedEvent final : public Event
	{
	public:
		AssetID MapImage = uavpf::cBadID;

		TiffMapLoadedEvent(AssetID mapImage)
			: MapImage(mapImage)
		{}
	};

	class StartPathFindingEvent final : public Event
	{};

	class CancelPathFindingEvent final : public Event
	{};	

	class PathFoundEvent final : public Event
	{};
}

