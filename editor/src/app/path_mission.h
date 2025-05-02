#pragma once

#include <glm/glm.hpp>
#include <uavpf/uavpf.h>

namespace editor
{
	class Path
	{
	public:
		void AddCoordinate(const glm::vec2& coordinate, float elevation);

		void Clear();
		const std::vector<glm::vec3>& GetCoordinates() const;

	private:
		std::vector<glm::vec3> mCoordinates;
	};

	class PathMission
	{
	public:
		enum class PathStatus
		{
			None,
			Found,
			NotFound,
		};

		void SetHeightMap(uavpf::HeightMap heightMap);

		void SpecifyGrid(uavpf::NavGridSpecification spec);
		const uavpf::NavGrid& GetNavGrid() const;

		uavpf::CostCollection& GetCosts();

		void SetRelativeStart(glm::vec2 relativeStart);
		glm::vec2 GetRelativeStart() const;

		void SetRelativeEnd(glm::vec2 relativeEnd);
		glm::vec2 GetRelativeEnd() const;

		void SetStart(glm::ivec2 start);
		glm::ivec2 GetStart() const;

		void SetEnd(glm::ivec2 end);
		glm::ivec2 GetEnd() const;

		void SetMinElevation(float elevation);
		float GetMinElevation() const;

		void SetStatus(PathStatus status);
		PathStatus GetStatus() const;

		Path& GetPath();

	private:
		uavpf::HeightMap mHeightMap;
		uavpf::NavGrid mGrid;
		uavpf::CostCollection mCosts;

		glm::vec2 mRelativeStart{};
		glm::vec2 mRelativeEnd{};
		float mMinElevation = 0.1f;

		Path mPath;
		PathStatus mStatus = PathStatus::None;
	};
}

