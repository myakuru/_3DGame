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

	// 更新/描画フック
	void CaptureAfterImage();
	void DrawAfterImages();

	// 残像の状態設定(残像を使用するか？、残像の最台数、残像を何秒間表示するか、カラー)
	void AddAfterImage(bool _flg = false, int _max = 0, int _nterval = 0, const Math::Color& _color = {0,0,0,0})
	{
		m_afterImageEnable = _flg;
		m_afterImageMax = _max;
		m_afterImageInterval = _nterval;
		m_afterImageColor = _color;
	}

private:

	void ApplyHorizontalMove(const Math::Vector3& inputMove, float deltaTime);

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
	bool m_afterImageEnable = false;
	int  m_afterImageMax = 1;
	int  m_afterImageInterval = 1; // 何フレームごとに保存するか
	int  m_afterImageCounter = 0;	// カウンタ
	Math::Color m_afterImageColor = {0,1,1,0.1f}; // 基本色（半透明白）


	std::deque<AfterImageFrame> m_afterImages;

	// 描画用テンポラリ Work（元モデルと同じ Data を参照）
	std::unique_ptr<KdModelWork> m_afterImageWork;

	Math::Vector3 m_prevPosition{};          // 前フレームのワールド位置
	float         m_forwardRayYOffset = 0.35f; // プレイヤー中心(腰程度)の高さ
	float         m_forwardRayMargin = 0.02f; // 壁手前で残すマージン
	float         m_forwardRayExtra = 0.05f; // 余剰距離(浮動小数ヒット安定用)

	float kBumpSphereRadius = 0.2f; // 壁にめり込むのを防ぐための球の半径
	float kBumpSphereYOffset = 0.3f; // プレイヤー中心(腰程度)の高さ	
	float kCollisionMargin = 0.01f; // 壁にめり込まないための余白

};