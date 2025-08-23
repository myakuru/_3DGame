#pragma once
#include "../CharacterBase.h"
#include"PlayerConfig.h"
class Enemy;
class Katana;
class PlayerStateBase;
class Player :public CharaBase
{
public:

	// クラスごとに一意なTypeIDを持たせる
	static const uint32_t TypeID;
	Player() { m_typeID = TypeID; };
	~Player() override = default;

	void Init() override;
	void PreUpdate() override;
	void SkirtUpdate();
	void Update() override;
	void UpdateAttack();
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

	PlayerConfig& GetPlayerConfig() { return m_playerConfig; }
	const Math::Vector3& GetPosition() const { return m_position; }

	struct PlayerStatus
	{
		int hp = 100;		// ヒットポイント
		int attack = 20;	// 攻撃力
		int hpMax = 100;	// 最大ヒットポイント
	};

	// ダメージを受けた時の処理
	void TakeDamage(int damage)
	{
		m_status.hp -= damage;
		if (m_status.hp < 0) m_status.hp = 0;
	}

	void HitCheck(bool _isHit)
	{
		m_isHit = _isHit;
	}

	bool m_onceEffect = false;

private:

	Math::Vector3 m_moveDirection = Math::Vector3::Zero; // 移動方向
	
	Math::Vector3 m_lastMoveDirection = Math::Vector3::Zero;

	std::weak_ptr<Katana>	m_katana;
	std::weak_ptr<Enemy>	m_enemy; // 敵の参照

	struct KdModelWork::Node* m_armatureNode = nullptr; // ヒップノード

	PlayerStatus m_status;
	bool m_isHit = false;						// ヒット判定用

	float m_attackBossEnemyRadius = 1.0f;

	PlayerConfig m_playerConfig;

};