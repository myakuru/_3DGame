#pragma once
class TPSCamera;
class FPSCamera;
class ImGuiManager : public KdGameObject
{
public:
	ImGuiManager() = default;
	~ImGuiManager() override = default;

	void Init() override;

	void ImGuiUpdate();

	void Hierarchy();

	void MainMenuBar();

	void TreeNode();

	// インスペクターの表示のみ
	void ShowInspector();

	void ShowGameScene();

	std::string ImSelectClass() const;

	void InGuiSceneSelect() const;

	void ImGuiSelecetCamera();

	KdRenderTargetPack GetRenderTargetChanger() { return m_renderTargetChanger; }

	bool GetGameSceneInMouse() const { return m_gameSceneInMouse; }

	Math::Vector2 GetWindowSize() const { return m_windowSize; }

	bool GetShowEffect() const { return m_showEffect; }

	bool GetShowDebugWireFrame() const { return m_showDebugWireFrame; }

private:

	std::weak_ptr<TPSCamera> m_tpsCamera;
	std::weak_ptr<FPSCamera> m_fpsCamera;

	// FPSカメラのウィークポインタを取得する。
	std::shared_ptr<KdCamera> GetActiveCamera();

	// 開きたいオブジェクトを記録する変数
	std::shared_ptr<KdGameObject> m_openObject = nullptr;

	std::shared_ptr<KdTexture> m_texture = nullptr;

	KdRenderTargetPack m_renderTargetChanger;

	// 本画像のを半分にする
	ImVec2 m_gameSceneSize;

	ImVec2 m_winSize{};
	float m_width = 0.0f;
	float m_height = 0.0f;

	float offsetX = 0.0f;
	float offsetY = 0.0f;

	Math::Vector2 m_windowSize = Math::Vector2(1920, 1080);

	bool m_gameSceneInMouse = true;

	bool m_showEffect = true;


	bool m_showDebugWireFrame = false;

	std::string m_modelPath = "Asset/";

};