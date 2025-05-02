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
		FillMode Fill = FillMode::Solid;
		CullMode Cull = CullMode::Back;
	};
}

