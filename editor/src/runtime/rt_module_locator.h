#pragma once

#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>

namespace editor
{
	class RtModuleEntry
	{
	public:
		virtual ~RtModuleEntry() = default;
	};

	template<
		typename ModuleT
	>
	class TRtModuleEntry final : public RtModuleEntry
	{
	public:
		using ModulePtr = std::add_pointer_t<ModuleT>;

		TRtModuleEntry(ModulePtr rtModule)
			: mRtModule(rtModule)
		{}

		ModulePtr GetModule() const
		{
			return mRtModule;
		}

	private:
		ModulePtr mRtModule = nullptr;
	};

	class RtModuleLocator
	{
	public:
		template<typename ModuleT>
		static void Register(std::add_pointer_t<ModuleT> rtModule)
		{
			auto entry = std::make_unique<TRtModuleEntry<ModuleT>>(rtModule);
			GetInstance().mRegisteredModules[typeid(ModuleT)] = std::move(entry);
		}

		template<typename ModuleT>
		static void Unregister()
		{
			GetInstance().mRegisteredModules[typeid(ModuleT)].reset();
		}

		template<typename ModuleT>
		static std::add_pointer_t<ModuleT> Locate()
		{
			RtModuleEntry* rtModule = GetInstance().mRegisteredModules.at(typeid(ModuleT)).get();
			auto* actualRtModule = dynamic_cast<TRtModuleEntry<ModuleT>*>(rtModule);
			return actualRtModule->GetModule();
		}

	private:
		RtModuleLocator() = default;	

		RtModuleLocator(const RtModuleLocator& other) = delete;	
		RtModuleLocator& operator=(const RtModuleLocator& other) = delete;	

		static RtModuleLocator& GetInstance();

		std::unordered_map<
			std::type_index,
			std::unique_ptr<RtModuleEntry>
		> mRegisteredModules;
	};
}

