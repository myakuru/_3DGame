#pragma once
#include"../Utility/SelectDraw3dModel.h"
#include"../../../MyFramework/State/StateManager/StateManager.h"
class PlayerStateBase;
class CharaBase:public SelectDrawObject
{
public:
	CharaBase() = default;
	~CharaBase() override = default;

	/// <summary>
	/// 移動ベクトルを設定します。
	/// </summary>
	/// <param name="_move">新しい移動ベクトル。</param>
	void SetIsMoving(Math::Vector3 _move) { m_movement = _move; }
	Math::Vector3 &GetMovement() { return m_movement; }

	/// <summary>
	/// ステートで変更するアニメーションモデルを取得する関数
	/// </summary>
	std::shared_ptr<KdModelWork> GetAnimeModel() const { return m_modelWork; }
	std::shared_ptr<KdAnimator> GetAnimator() const { return m_animator; }
	bool& AnimeSetFlg() { return IsAnimeSet; }

protected:

	/// <summary>
	/// KdGameObjectから派生された関数
	/// </summary>
	void Init() override;
	void DrawToon() override;
	void Update() override;
	bool ModelLoad(std::string _path) override;
	void ImGuiInspector() override;
	void JsonInput(const nlohmann::json& _json) override;
	void JsonSave(nlohmann::json& _json) const override;

	/// <summary>
	/// ステート関係の関数
	/// </summary>
	virtual void StateInit() {};
	virtual void ChangeState(std::shared_ptr<PlayerStateBase> _state) {};
	StateManager m_stateManager;

	/// <summary>
	/// アニメーション関係
	/// </summary>
	std::shared_ptr<KdModelWork>	m_modelWork = std::make_shared<KdModelWork>();
	std::shared_ptr <KdAnimator>	m_animator = std::make_shared<KdAnimator>();
	bool IsAnimeSet = false;


	// 重力
	float m_gravity = 0.0f;
	float m_gravitySpeed = 0.0f;

	// 60fps換算にするようの変数
	float m_fixedFrameRate = 0.0f;

	// 行列関係
	Math::Matrix m_mRotation = Math::Matrix::Identity;

	/// <summary>
	/// 移動関係
	/// </summary>
	Math::Vector3 m_movement = Math::Vector3::Zero;

	KdTrailPolygon m_trailPolygon; // トレイルポリゴン

};