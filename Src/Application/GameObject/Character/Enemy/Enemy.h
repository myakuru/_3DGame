#pragma once
class Player;
#include "../CharacterBase.h"
class Enemy :public CharaBase
{
public:
	// クラスごとに一意なTypeIDを持たせる
	static const uint32_t TypeID;
	Enemy() { m_typeID = TypeID; }
	~Enemy() override = default;

	void Init() override;
	void Update() override;
	void DrawLit() override;
	void PostUpdate() override;
	void ImGuiInspector() override;
	void JsonInput(const nlohmann::json& _json) override;
	void JsonSave(nlohmann::json& _json) const override;

	void UpdateQuaternion(Math::Vector3& _moveVector) override;

	void StateInit();
	void ChangeState(std::shared_ptr<EnemyStateBase> _state);

	const std::weak_ptr<Player>& GetPlayerWeakPtr() const
	{
		return m_wpPlayer;
	}

	void UpdateAttack();

	// ダメージを受ける
	void Damage(int _damage);

	void HitCheck(bool _isHit)
	{
		m_isAtkPlayer = _isHit;
	}

	bool GetHitCheck() const
	{
		return m_isAtkPlayer;
	}

	// Enemyがダメージ受けたときのセッター
	bool EnemyHit() const
	{
		return m_isHit;
	}

	void SetEnemyHit(bool _hit)
	{
		m_isHit = _hit;
	}

	struct EnemyStatus
	{
		int hp = 100;				// 体力
		int maxHp = 100;			// 最大体力
		int attack = 10;			// 攻撃力
	};

	bool m_onceEffect = false;

private:

	std::weak_ptr<Player> m_wpPlayer;
	bool m_Expired = false;				// 敵を消滅させるかどうか
	bool m_isHit = false;				// ヒット判定
	bool m_isAtkPlayer = false;
	float m_dissever = 0.0f;			// 敵のディゾルブ値

	float m_attackRadius = 1.5f;	// 攻撃判定の半径
	float m_attackFrame = 0.0f;	// 攻撃判定フレーム

	EnemyStatus m_status;			// 敵のステータス
};