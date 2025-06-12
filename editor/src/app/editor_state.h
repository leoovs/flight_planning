#pragma once

#include "app/app_events.h"
#include "app/asset_storage.h"
#include "app/editor_state_kind.h"
#include "event/event_publisher.h"
#include "event/event_subscriber.h"
#include "platform/platform_events.h"
#include "rendering/overlay_renderer.h"
#include "rendering/terrain_renderer.h"

namespace editor
{
	class AppService;
	class EditorApp;

	class EditorState
	{
	public:
		virtual ~EditorState() = default;

		virtual EditorStateKind GetKind() const = 0;

		virtual void OnAttach(EditorApp* app) = 0;
		virtual void OnDetach() = 0;

		virtual void Update() {}
		virtual void Render() {}
		virtual void ShowMenu() {} 
		virtual void ShowUI() {}
	};

	class Editor_Idle final : public EditorState
	{
	public:
		~Editor_Idle() override = default;

		EditorStateKind GetKind() const override;

		void OnAttach(EditorApp* app) override;
		void OnDetach() override;

		void ShowMenu() override; 

	private:
		bool OnTiffMapRequested(const TiffMapRequestedEvent& event);
		bool OnTiffMapLoaded(const TiffMapLoadedEvent& event);

		void LoadTiffMap(const std::filesystem::path& path);

		EditorApp* mApp = nullptr;
		EventSubscriber mSubscriber;
		EventPublisher mPublisher;
		std::atomic<AssetID> mMapImage = uavpf::cBadID;
	};

	class Editor_PathBuilder final : public EditorState
	{
	public:
		enum class State
		{
			Idle,
			Preparing,
			PathFinding,
		};

		~Editor_PathBuilder() override = default;

		EditorStateKind GetKind() const override;

		void OnAttach(EditorApp* app) override;
		void OnDetach() override;

		void Update() override;
		void Render() override;
		void ShowMenu() override;
		void ShowUI() override;

	private:
		void Render_PathMissionTargets();
		void Render_Path();
		void Render_Notams();

		void Show_CloseMenu();
		void Show_PathMissionPanel();
		void Show_CostCollection();
		void Show_CostMenu(ptrdiff_t iCost);
		void Show_NotamMenu(uavpf::NotamCost* cost);
		void Show_ScenePanel();
		void Show_MapPanel();

		void UpdateProgressMessage();
		void InitializePathMission();
		void InitializeTerrainMesh();

		void LoadTerrainMesh();
		void SetupRenderBuffers(int32_t width, int32_t height);
		void SetupRenderers();
		void HandleCameraInput();
		void ResetTerrainScaling();
		glm::vec4 NavCoordToWorldCoord(const glm::vec3& navCoord) const;

		bool OnStartPathFinding(const StartPathFindingEvent& event);
		bool OnMouseMove(const MouseMovementEvent& event);

		State mState = State::Preparing;
		EditorApp* mApp = nullptr;
		EventSubscriber mSubscriber;
		EventPublisher mPublisher;

		std::string mProgressMessage = "";
		std::string mProgressTitle = "Preparing PathBuilder";
		std::string mProgressChar = "\n*";
		float mProgressSeconds = 0.0f;

		uavpf::TerrainMesh mTerrainMesh;
		std::unique_ptr<TerrainRenderMesh> mTerrainRenderMesh;

		std::shared_ptr<Texture2D> mColorBuffer;
		std::shared_ptr<Texture2D> mDepthBuffer;
		std::unique_ptr<TerrainRenderer> mTerrainRenderer;
		std::unique_ptr<OverlayRenderer> mOverlayRenderer;

		bool mIsSceneFocused = false;
		bool mCanOperateCamera = false;
		std::atomic_bool mCanLoadMesh = false;
		std::atomic_bool mStopPathFinding = false;
		Camera mFreeCamera;
		Camera mTopDownCamera;
	};
}
