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
	int attack = 3000;	// 攻撃力
	int hpMax = 1000;	// 最大ヒットポイント
	int chargeCount = 0; // チャージ攻撃の回数
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
	void DrawUnLit() override;
	void Update() override;
	// 攻撃の当たり判定(攻撃半径、攻撃距離、攻撃回数、攻撃間隔、カメラシェイクの強さ、カメラシェイクの時間)
	void UpdateAttackCollision(float _radius = 10.f,float _distance = 1.1f,
		int _attackCount = 5,float _attackTimer = 0.3f,
		Math::Vector2 _cameraShakePow = { 0.3f,0.3f },float _cameraTime = 0.3f);

	// 当たり判定リセット
	void ResetAttackCollision()
	{
		m_chargeAttackCount = 0;
		m_chargeAttackTimer = 0.0f;
		m_isChargeAttackActive = false;
		m_onceEffect = false;
	}

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
	const std::list<std::weak_ptr<Enemy>>& GetEnemies() const { return m_enemies; }

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


	PlayerStatus& GetPlayerStatus() { return m_status; }

	const Math::Vector2& GetCameraShakePower() const { return m_cameraShakePower; }
	float GetCameraShakeTime() const { return m_cameraShakeTime; }


	// 残像処理

	// 更新/描画フック
	void CaptureAfterImage();
	void DrawAfterImages();

	// 残像の状態設定(残像を使用するか？、残像の最台数、残像を何秒間表示するか、カラー)
	void AddAfterImage(bool _flg = false, int _max = 0, float _nterval = 0, const Math::Color& _color = {0,0,0,0},float dissever = 0.0f)
	{
		m_afterImageEnable = _flg;
		m_afterImageMax = _max;
		m_afterImageInterval = _nterval;
		m_afterImageColor = _color;
		m_dissever = dissever;
	}

	// 残像のリセット
	void ResetAfterImage()
	{
		m_afterImages.clear();
		m_afterImageCounter = 0;
	}

	void SetHitCheck(bool _isHit)
	{
		m_isHit = _isHit;
	}

	bool GetHitCheck() const
	{
		return m_isHit;
	}

private:

	void ApplyHorizontalMove(const Math::Vector3& inputMove, float deltaTime);
	void ApplyPushWithCollision(const Math::Vector3& rawPush);


	Math::Vector3 m_moveDirection = Math::Vector3::Zero;		// 移動方向
	Math::Vector3 m_lastMoveDirection = Math::Vector3::Zero;	// 最後に移動した方向
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

	// 参照
	std::weak_ptr<Katana>	m_katana;							// カタナの参照
	std::weak_ptr<WeaponKatanaScabbard>	m_scabbard;				// カタナの参照
	std::weak_ptr<Enemy>	m_enemy;							// 敵の参照
	std::list<std::weak_ptr<Enemy>> m_enemies;					// 敵のリスト


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
	float  m_afterImageInterval = 0.1f; // 何フレームごとに保存するか
	float  m_afterImageCounter = 0.0f;	// カウンタ
	Math::Color m_afterImageColor = {0,1,1,0.1f}; // 基本色（半透明白）

	bool m_isHit = false;						// ヒット判定用

	std::deque<AfterImageFrame> m_afterImages;

	// 描画用テンポラリ Work（元モデルと同じ Data を参照）
	std::unique_ptr<KdModelWork> m_afterImageWork;


};