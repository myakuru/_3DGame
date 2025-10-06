#pragma once
class PlayerCamera;
class BaseScene
{
public :

	BaseScene() { Init(); }
	virtual ~BaseScene() {}

	void PreUpdate();
	void Update();
	void PostUpdate();

	void PreDraw();
	void Draw();
	void DrawSprite();
	void DrawDebug();

	virtual void JsonInput([[maybe_unused]] const nlohmann::json& _json) {/*基底クラスではなにもしない*/ }
	virtual void JsonSave([[maybe_unused]] nlohmann::json& _json) const {/*基底クラスではなにもしない*/ }
	virtual void LoadSceneSettingsFromJson([[maybe_unused]] const std::string& filePath) {/*基底クラスではなにもしない*/ }
	virtual void SaveSceneSettingsToJson([[maybe_unused]] const std::string& filePath) const {/*基底クラスではなにもしない*/ }

	virtual void DrawImGui() {/*基底クラスではなにもしない*/}

	// オブジェクトリストを取得
	std::list<std::shared_ptr<KdGameObject>>& GetObjList()
	{
		return m_objList;
	}
	
	std::list<std::shared_ptr<KdGameObject>>& WorkObjList()
	{
		return m_objList;
	}

	// オブジェクトリストに追加
	void AddObject(const std::shared_ptr<KdGameObject>& _obj)
	{
		m_objList.push_back(_obj);
	}
	
	// カメラオブジェクトリストを取得
	void AddCameraObject(const std::shared_ptr<KdGameObject>& _obj)
	{
		m_CameraObjList.push_back(_obj);
	}
	// カメラオブジェクトリストを取得
	std::list<std::shared_ptr<KdGameObject>>& GetCameraObjList()
	{
		return m_CameraObjList;
	}

	void AddMapObject(const std::shared_ptr<KdGameObject>& _obj)
	{
		m_MapObjectList.push_back(_obj);
	}

	std::list<std::shared_ptr<KdGameObject>>& GetMapObjectList()
	{
		return m_MapObjectList;
	}

	virtual std::string GetSceneName() const = 0;

	const KdRenderTargetPack& GetRenderTargetPack() const
	{
		return m_renderTargetPack;
	}

	const KdRenderTargetPack& GetRenderTargetUIPack() const
	{
		return m_renderTargetUIPack;
	}

protected :

	// デバッグGUI表示時のみRT切り替え、終了時に自動Undo
	template<typename Func>
	void WithDebugRenderTarget(Func drawFunc)
	{
		bool changedRT = false;
		if (KdDebugGUI::Instance().ShowImGUiFlg())
		{
			m_renderTargetChanger.ChangeRenderTarget(m_renderTargetPack);
			changedRT = true;
		}

		drawFunc();

		if (changedRT)
		{
			m_renderTargetChanger.UndoRenderTarget();
		}
	}

	// 継承先シーンで必要ならオーバーライドする
	virtual void Event();
	virtual void Init();

	// PostProcess用パラメータ
	float m_brightThreshold = 0.9f;
	bool m_fogEnable = true;
	bool m_fogUseRange = false;
	Math::Vector3 m_fogColor = { 0.7f, 0.7f, 0.7f };
	Math::Vector3 m_highFogColor = { 0.7f, 0.7f, 0.7f };
	float m_highFogHeight = 0.0f;
	float m_lowFogHeight = 0.0f;
	float m_highFogDistance = 0.0f;
	Math::Vector3 m_directionalLightDir = { 1,1,1 };
	Math::Vector3 m_directionalLightColor = { 1.0f,1.0f,1.0f };
	Math::Vector4 m_anviLightColor = { 0.2f,0.2f,0.2f,0.2f };

	Math::Vector2 m_lightingArea = { 1.0f, 1.0f };
	float m_dirLightHeight = 1.0f;

	float m_fogDensity = 0.005f;

	std::weak_ptr<PlayerCamera> m_playerCamera;

	KdRenderTargetChanger m_renderTargetChanger;
	KdRenderTargetPack m_renderTargetPack;

	// UI用のレンダーターゲット切り替えるやつ
	KdRenderTargetChanger m_renderTargetUIChanger;
	// UIのテクスチャいれるやつ
	KdRenderTargetPack m_renderTargetUIPack;

	int m_gameWindowSizeX = 0;
	int m_gameWindowSizeY = 0;


	// 全オブジェクトのアドレスをリストで管理
	std::list<std::shared_ptr<KdGameObject>> m_objList;
	std::list<std::shared_ptr<KdGameObject>> m_CameraObjList;
	std::list<std::shared_ptr<KdGameObject>> m_MapObjectList;
	std::list<std::shared_ptr<KdGameObject>> m_CollisionList;
	std::list<std::shared_ptr<KdGameObject>> m_drawObjectList;
};
