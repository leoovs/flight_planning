#pragma once

#include <string_view>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

namespace uavpf
{
	class NavGrid;
	class NavNode;

	enum class AStarCostKind
	{
		Unknown,
		Distance,
		Slope,
		Notam,
		Collection,
	};

	std::string_view GetAStarCostKindString(AStarCostKind kind);

	class AStarCost
	{
	public:
		virtual ~AStarCost() = default;
		
		virtual void SetGrid(const NavGrid* grid) = 0;

		virtual AStarCostKind GetKind() const = 0;

		virtual float CalculateCost(const NavNode* current, const NavNode* neighbour) const = 0;
	};

	class DistanceCost final : public AStarCost
	{
	public:
		~DistanceCost() override = default;

		void SetGrid(const NavGrid* grid) override;

		AStarCostKind GetKind() const override;

		float CalculateCost(const NavNode* current, const NavNode* neighbour) const override;

	private:
		const NavGrid* mGrid = nullptr;
	};

	class SlopeCost final : public AStarCost
	{
	public:
		void SetGrid(const NavGrid* grid) override;

		AStarCostKind GetKind() const override;

		float CalculateCost(const NavNode* current, const NavNode* neighbour) const override;
	
	private:
		const NavGrid* mGrid = nullptr;
	};

	class NotamCost final : public AStarCost
	{
	public:
		void SetGrid(const NavGrid* grid) override;
		AStarCostKind GetKind() const override;
		float CalculateCost(const NavNode* current, const NavNode* neighbour) const override;

		glm::vec2& GetCenter();
		glm::ivec2 GetNavSpaceCenter();

		float& GetRadius();
		int32_t GetRadiusA() const;
		int32_t GetRadiusB() const;

	private:
		glm::vec2 mCenter{ 0.0f };
		float mRadius = 0.0f;

		const NavGrid* mGrid = nullptr;
	};

	class CostCollection final : public AStarCost
	{
	public:
		void SetGrid(const NavGrid* grid) override;
		AStarCostKind GetKind() const override;
		float CalculateCost(const NavNode* current, const NavNode* neighbour) const override;

		size_t GetSize() const;
		void Erase(ptrdiff_t iCost);
		void Clear();

		AStarCost* GetCost(ptrdiff_t iCost);
		float& GetWeight(ptrdiff_t iCost);

		void PushBack(std::unique_ptr<AStarCost> cost, float weight = 1.0f);

		template<typename CostT, typename... ArgsT>
		CostCollection& Add(float weight, ArgsT&&... args)
		{
			PushBack(std::make_unique<CostT>(std::forward<ArgsT>(args)...), weight);
			return *this;
		}

	private:
		std::vector<std::unique_ptr<AStarCost>> mCosts;
		std::vector<float> mWeights;
		const NavGrid* mGrid = nullptr;
	};
}
