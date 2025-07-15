#pragma once
// ゲーム上に存在するすべてのオブジェクトの基底となるクラス
class KdGameObject : public std::enable_shared_from_this<KdGameObject>
{
public:

	// どのような描画を行うのかを設定するTypeID：Bitフラグで複数指定可能
	enum class DrawTypeID
	{
		eDrawTypeLit = 1 << 0,
		eDrawTypeUnLit = 1 << 1,
		eDrawTypeBright = 1 << 2,
		eDrawTypeUI = 1 << 3,
		eDrawTypeDepthOfShadow = 1 << 4,
	};

	KdGameObject() = default;
	virtual ~KdGameObject() { Release(); }

	// 生成される全てに共通するパラメータに対する初期化のみ
	virtual void Init();

	virtual void PreUpdate() {}
	virtual void Update() {}
	virtual void PostUpdate() {}
	virtual void CameraUpdate() {}

	// それぞれの状況で描画する関数
	virtual void GenerateDepthMapFromLight() {}
	virtual void PreDraw() {}
	virtual void DrawLit() {}
	virtual void DrawUnLit() {}
	virtual void DrawEffect() {}
	virtual void DrawBright() {}
	virtual void DrawSprite() {}
	virtual void DrawToon() {}
	virtual void DrawDebug();

	virtual void SetAsset(const std::string&) {}

	virtual void SetPos(const Math::Vector3& pos) { m_mWorld.Translation(pos); }
	virtual Math::Vector3 GetPos() const { return m_mWorld.Translation(); }

	// 拡大率を変更する関数
	void SetScale(float scalar);
	virtual void SetScale(const Math::Vector3& scale);
	virtual Math::Vector3 GetScale() const;

	const Math::Matrix& GetMatrix() const { return m_mWorld; }

	virtual bool IsExpired() const { return m_isExpired; }
	virtual void SetExpired(bool isExpired) { m_isExpired = isExpired; }

	virtual bool IsVisible()	const { return false; }
	virtual bool IsRideable()	const { return false; }

	// 視錐台範囲内に入っているかどうか
	virtual bool CheckInScreen(const DirectX::BoundingFrustum&) const { return false; }

	// カメラからの距離を計算
	virtual void CalcDistSqrFromCamera(const Math::Vector3& camPos);

	float GetDistSqrFromCamera() const { return m_distSqrFromCamera; }

	UINT GetDrawType() const { return m_drawType; }

	bool Intersects(const KdCollider::SphereInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults);
	bool Intersects(const KdCollider::BoxInfo& targetBox, std::list<KdCollider::CollisionResult>* pResults);
	bool Intersects(const KdCollider::RayInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults);

	// 自分で追加したもの
	bool SelectObjectIntersects(const KdCollider::RayInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults);

	//===================================================================
	// 自分で追加したもの
	//===================================================================

	// 各オブジェクトのデーターをここでJsonから取ってくる関数
	virtual void JsonInput(const nlohmann::json& _json);

	// 各オブジェクトのデーターをここでJsonに保存する関数
	virtual void JsonSave(nlohmann::json&_json) const;

	virtual bool ModelLoad(std::string _path);

	// 各オブジェクトのImGuiインスペクターを実装する関数
	virtual void ImGuiInspector();

	void ImGuiSelectGltf();

protected:

	void Release() {}

	// 描画タイプ・何の描画を行うのかを決める / 最適な描画リスト作成用
	UINT m_drawType = 0;

	// カメラからの距離
	float m_distSqrFromCamera = 0;

	// 存在消滅フラグ
	bool m_isExpired = false;

	// 3D空間に存在する機能
	Math::Matrix	m_mWorld;

	// 当たり判定クラス
	std::unique_ptr<KdCollider> m_pCollider = nullptr;

	// デバッグ情報クラス
	std::unique_ptr<KdDebugWireFrame> m_pDebugWire = nullptr;


	//自分で追加したもの

	// jsonでクラスの名前を保存する変数
	std::string m_className = "Name";

	// モデルのロードパス
	std::string m_path = "Asset/Models/Sky/Sky.gltf";

	// モデルの初期化
	std::shared_ptr<KdModelData> m_model = std::make_shared<KdModelData>();
	// ポリゴンの初期化
	std::shared_ptr<KdSquarePolygon> m_polygon = std::make_shared<KdSquarePolygon>();
	// 2Dテクスチャの初期化
	std::shared_ptr <KdTexture> m_texture = std::make_shared<KdTexture>();

	// 位置
	Math::Vector3 m_pos = Math::Vector3::Zero;
	// 拡大率
	Math::Vector3 m_scale = Math::Vector3::One;
	//Math::Quaternion m_rot = Math::Quaternion::Identity; // 回転
	// 回転角度（デグリー）
	Math::Vector3 m_deg = Math::Vector3::Zero;
	// カラー
	Math::Color m_color = {1,1,1,1};

};
