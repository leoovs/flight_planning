#pragma once

#include <memory>
#include <vector>
#include <limits>

namespace uavpf::experimental
{
	struct NavCell;

	class StepCost
	{
	public:
		static constexpr float cInfinite = std::numeric_limits<float>::infinity();

		virtual ~StepCost() = default;

		virtual float Evaluate(const NavCell& src, const NavCell& dst) const;
	};

	class DistanceCost final : public StepCost
	{
	public:
		~DistanceCost() override = default;

		float Evaluate(const NavCell& src, const NavCell& dst) const override;
	};

	class ClimbCost final : public StepCost
	{
	public:
		~ClimbCost() override = default;

		float Evaluate(const NavCell& src, const NavCell& dst) const override;
	};

	class ComplexCost final : public StepCost
	{
	public:
		using WeightedCost = std::pair<std::unique_ptr<StepCost>, float>;

		~ComplexCost() override = default;

		float Evaluate(const NavCell& src, const NavCell& dst) const override;

		size_t GetCostCount() const;

		StepCost& GetCost(ptrdiff_t iCost);
		const StepCost& GetCost(ptrdiff_t iCost) const;

		float& GetWeight(ptrdiff_t iCost);
		const float& GetWeight(ptrdiff_t iCost) const;

		void RemoveCost(ptrdiff_t iCost);
		void ClearCosts();

	private:
		std::vector<WeightedCost> mCosts;
	};
}


