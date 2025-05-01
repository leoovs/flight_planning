#pragma once

#include <string_view>

namespace uavpf
{
	class NavGrid;
	class NavNode;

	class AStarCost
	{
	public:
		virtual ~AStarCost() = default;
		
		virtual void SetGrid(const NavGrid* grid) = 0;
		virtual void SetTarget(const NavNode* target) = 0;

		virtual std::string_view GetName() const = 0;

		virtual float CalculateCost(const NavNode* current, const NavNode* neighbour) const = 0;
	};

	class ContourMatchingCost final : public AStarCost
	{
	public:
		~ContourMatchingCost() override = default;
		
		void SetGrid(const NavGrid* grid) override;
		void SetTarget(const NavNode* target) override;

		std::string_view GetName() const override;

		float CalculateCost(const NavNode* current, const NavNode* neighbour) const override;

	private:
		const NavGrid* mGrid = nullptr;
		const NavNode* mTarget = nullptr;
	};

	class ElevationConservingCost : public AStarCost
	{
	public:
		ElevationConservingCost();
		~ElevationConservingCost() override = default;
		
		void SetGrid(const NavGrid* grid) override;
		void SetTarget(const NavNode* target) override;

		std::string_view GetName() const override;

		float CalculateCost(const NavNode* current, const NavNode* neighbour) const override;

	private:
		ContourMatchingCost mDistanceHeuristic;	
		const NavGrid* mGrid = nullptr;
		const NavNode* mTarget = nullptr;
	};
}

