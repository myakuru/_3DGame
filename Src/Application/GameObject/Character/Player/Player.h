#pragma once
#include "../CharacterBase.h"
#include"PlayerConfig.h"
class Enemy;
class Katana;
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
	void SkirtUpdate();
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
	const std::weak_ptr<Enemy>& GetEnemy() const { return m_enemy; }

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

private:

	Math::Vector3 m_moveDirection = Math::Vector3::Zero; // 移動方向
	
	Math::Vector3 m_lastMoveDirection = Math::Vector3::Zero;

	std::weak_ptr<Katana>	m_katana;	// カタナの参照
	std::weak_ptr<Enemy>	m_enemy;	// 敵の参照

	struct KdModelWork::Node* m_armatureNode = nullptr; // ヒップノード

	PlayerStatus m_status;

	bool m_nowAvoid = false;				// 回避中かどうか

	float m_attackBossEnemyRadius = 2.0f;
	float m_avoidStartTime = 0.0f; // 回避開始タイム

	int m_chargeAttackCount = 0;      // 何回ダメージを与えたか
	float m_chargeAttackTimer = 0.0f; // 経過時間
	bool m_isChargeAttackActive = false; // 連続攻撃中か

	PlayerConfig m_playerConfig;

};