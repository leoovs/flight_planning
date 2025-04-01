#pragma once

namespace editor
{
	enum class FillMode
	{
		Solid,
		Wireframe,
	};

	enum class CullMode
	{
		None,
		Front,
		Back,
	};

	struct RasterizerState
	{
		FillMode FillMode = FillMode::Solid;
		CullMode CullMode = CullMode::Back;
	};
}

