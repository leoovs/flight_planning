#pragma once

#include <cstddef>

namespace editor
{
	enum class AssetKind
	{
		HeightMap,
		Count_,
	};

	constexpr size_t operator+(AssetKind kind)
	{
		return static_cast<size_t>(kind);
	}

	class Asset
	{
	public:
		virtual ~Asset() = default;

		virtual AssetKind GetKind() const = 0;
	};
}

