#pragma once

#include <glm/glm.hpp>
#include <uavpf/uavpf.h>

namespace editor
{
	class PathMission
	{
	public:
		void SetHeightMap(uavpf::HeightMap heightMap);

		void SpecifyGrid(uavpf::NavGridSpecification spec);
		const uavpf::NavGrid& GetNavGrid() const;

		void SetStart(glm::vec2 start);
		glm::vec2 GetStart() const;

		void SetEnd(glm::vec2 end);
		glm::vec2 GetEnd() const;

	private:
		uavpf::HeightMap mHeightMap;
		uavpf::NavGrid mGrid;
		glm::vec2 mStart;
		glm::vec2 mEnd;
	};
}

