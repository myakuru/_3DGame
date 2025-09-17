#pragma once
#include"../Utility/SelectDraw3dModel.h"
#include"../../../MyFramework/State/StateManager/StateManager.h"
class EnemyStateBase;
class PlayerCamera;
class CharaBase:public SelectDraw3dModel
{
public:
	CharaBase() = default;
	~CharaBase() override = default;

	/// <summary>
	/// 移動ベクトルを設定します。
	/// </summary>
	/// <param name="_move">新しい移動ベクトル。</param>
	void SetIsMoving(Math::Vector3 _move) { m_movement = _move; }
	const Math::Vector3 &GetMovement() const { return m_movement; }
	Math::Matrix& GetRotationMatrix() { return m_mRotation; }
	void SetRotation(const Math::Quaternion& rotation) { m_rotation = rotation; }
	Math::Quaternion& GetRotationQuaternion() { return m_rotation; }

	const bool & GetMoving() const { return m_isMoving; }
	void SetPosition(const Math::Vector3& position) { m_position = position; }
	//const Math::Vector3& GetPosition() const { return m_position; }

	/// <summary>
	/// ステートで変更するアニメーションモデルを取得する関数
	/// </summary>
	std::shared_ptr<KdModelWork> GetAnimeModel();
	std::shared_ptr<KdAnimator> GetAnimator() const { return m_animator; }
	bool& AnimeSetFlg() { return IsAnimeSet; }
	void SetAnimeSpeed(float speed) { m_fixedFrameRate = speed; }

	/// <summary>
	/// ムーブベクトルに基づいてクォータニオンを更新
	/// </summary>
	/// <param name="_moveVector">クォータニオンの更新に使用するVector3の移動ベクトル。</param>
	virtual void UpdateQuaternion(Math::Vector3& _moveVector);

	void UpdateQuaternionDirect(const Math::Vector3& direction);

	std::weak_ptr<PlayerCamera> GetPlayerCamera() const { return m_playerCamera; }

protected:

	/// <summary>
	/// KdGameObjectから派生された関数
	/// </summary>
	void Init() override;
	void Update() override;
	void PostUpdate() override;
	bool ModelLoad(std::string _path) override;
	void ImGuiInspector() override;
	void JsonInput(const nlohmann::json& _json) override;
	void JsonSave(nlohmann::json& _json) const override;


	/// <summary>
	/// ステート関係の関数
	/// </summary>
	StateManager m_stateManager;

	/// <summary>
	/// アニメーション関係
	/// </summary>
	std::shared_ptr <KdAnimator>	m_animator = std::make_shared<KdAnimator>();
	bool IsAnimeSet = false;


	// 重力
	float m_gravity = 0.0f;
	float m_gravitySpeed = 0.0f;

	// 60fps換算にするようの変数
	float m_fixedFrameRate = 0.0f;

	float m_rotateSpeed = 0.0f; // 回転速度

	// 行列関係
	Math::Matrix m_mRotation = Math::Matrix::Identity;

	Math::Quaternion m_rotation = Math::Quaternion::Identity; // 回転用クォータニオン

	/// <summary>
	/// 移動関係
	/// </summary>
	Math::Vector3 m_movement = Math::Vector3::Zero;
	float m_moveSpeed = 0.0f; // 移動速度
	bool m_isMoving = false; // 移動中かどうか

	// カメラへの参照
	std::weak_ptr<PlayerCamera> m_playerCamera;

	DirectX::BoundingSphere sphere;

	KdTrailPolygon m_trailPolygon; // トレイルポリゴン

};