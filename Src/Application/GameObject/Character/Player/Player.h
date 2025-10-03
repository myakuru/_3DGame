#pragma once
#include "../CharacterBase.h"
#include"PlayerConfig.h"
class Enemy;
class Katana;
class WeaponKatanaScabbard;
class PlayerStateBase;
struct PlayerStatus
{
	int hp = 1000;		// ヒットポイント
	int attack = 2500;	// 攻撃力
	int hpMax = 1000;	// 最大ヒットポイント
};
class Player :public CharaBase
{
public:

	// クラスごとに一意なTypeIDを持たせる
	static const uint32_t TypeID;
	Player() { m_typeID = TypeID; }
	~Player() override = default;

	void Init() override;
	void PreUpdate() override;
	void PostUpdate() override;
	void DrawBright() override;
	void Update() override;
	void UpdateAttack();
	void UpdateChargeAttack();
	void ImGuiInspector() override;
	void JsonInput(const nlohmann::json& _json) override;
	void JsonSave(nlohmann::json& _json) const override;

	void StateInit();
	void ChangeState(std::shared_ptr<PlayerStateBase> _state);
	void UpdateMoveDirectionFromInput();

	const Math::Vector3& GetMoveDirection() const { return m_moveDirection; }
	Math::Vector3 GetLastMoveDirection() const { return m_lastMoveDirection; }
	void SetMoveDirection(const Math::Vector3& _moveDirection) { m_moveDirection = _moveDirection; }

	KdModelWork* GetModelWork() { return m_modelWork.get(); }

	const std::weak_ptr<Katana>& GetKatana() const { return m_katana; }
	const std::weak_ptr<WeaponKatanaScabbard>& GetScabbard() const { return m_scabbard; }
	const std::weak_ptr<Enemy>& GetEnemy() const { return m_enemy; }

	void SetAtkPlayer(bool flg) { m_isAtkPlayer = flg; }

	PlayerConfig& GetPlayerConfig() { return m_playerConfig; }

	// ダメージを受けた時の処理
	void TakeDamage(int damage)
	{
		m_status.hp -= damage;
		if (m_status.hp < 0) m_status.hp = 0;
	}

	bool m_onceEffect = false;

	void SetAvoidFlg(bool _flg) { m_nowAvoid = _flg; }
	bool GetAvoidFlg() const { return m_nowAvoid; }

	void SetAvoidStartTime(float time) { m_avoidStartTime = time; }
	float GetAvoidStartTime() const { return m_avoidStartTime; }

	bool m_isHit = false;						// ヒット判定用

	PlayerStatus& GetPlayerStatus() { return m_status; }

	const Math::Vector2& GetCameraShakePower() const { return m_cameraShakePower; }
	float GetCameraShakeTime() const { return m_cameraShakeTime; }


	// 残像処理

	// 残像の有効/設定
	void SetAfterImageEnable(bool enable) { m_afterImageEnable = enable; }
	void SetAfterImageMax(size_t max) { m_afterImageMax = (int)max; }
	void SetAfterImageInterval(int frameInterval) { m_afterImageInterval = frameInterval; }
	void SetAfterImageColor(const Math::Color& col) { m_afterImageColor = col; }

	// 更新/描画フック
	void CaptureAfterImage();   // PostUpdate などで呼ぶ
	void DrawAfterImages();     // Draw の前後で呼ぶ

private:

	Math::Vector3 m_moveDirection = Math::Vector3::Zero;		// 移動方向
	Math::Vector3 m_lastMoveDirection = Math::Vector3::Zero;	// 最後に移動した方向
	std::weak_ptr<Katana>	m_katana;							// カタナの参照
	std::weak_ptr<WeaponKatanaScabbard>	m_scabbard;				// カタナの参照
	std::weak_ptr<Enemy>	m_enemy;							// 敵の参照
	PlayerStatus m_status;										// プレイヤーのステータス
	bool m_nowAvoid = false;									// 回避中かどうか
	float m_attackBossEnemyRadius = 2.0f;						// ボスに攻撃する時の当たり判定の半径
	float m_avoidStartTime = 0.0f;								// 回避開始タイム
	int m_chargeAttackCount = 0;								// 何回ダメージを与えたか
	float m_chargeAttackTimer = 0.0f;							// 経過時間
	bool m_isChargeAttackActive = false;						// 連続攻撃中か
	Math::Vector2 m_cameraShakePower = Math::Vector2::Zero;		// カメラシェイクの強さ
	float m_cameraShakeTime = 0.0f;								// カメラシェイクの時間
	PlayerConfig m_playerConfig;								// プレイヤーの設定
	bool m_isAtkPlayer = false;									// プレイヤーと敵が接触したか どうか


	// 残像関連
	struct AfterImageFrame
	{
		std::vector<Math::Matrix> nodeWorlds; // 各ノードの worldTransform（モデルローカル空間）
		Math::Matrix ownerWorld = Math::Matrix::Identity; // その時点の m_mWorld
		float alpha = 1.0f; // このフレームの不透明度
	};

	// 残像設定・状態
	bool m_afterImageEnable = true;
	int  m_afterImageMax = 5;
	int  m_afterImageInterval = 3; // 何フレームごとに保存するか
	int  m_afterImageCounter = 0;	// カウンタ
	Math::Color m_afterImageColor = {0,1,1,0.5f}; // 基本色（半透明白）

	std::deque<AfterImageFrame> m_afterImages;

	// 描画用テンポラリ Work（元モデルと同じ Data を参照）
	std::unique_ptr<KdModelWork> m_afterImageWork;

};