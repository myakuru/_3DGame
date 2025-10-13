#pragma once
#include "../CharacterBase.h"
#include"../../HitDamage/HitDamage.h"
class Player;
class EnemySword;
class EnemyShield;
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

	const std::weak_ptr<EnemySword>& GetEnemySword() const
	{
		return m_enemySword;
	}

	const std::weak_ptr<EnemyShield>& GetEnemyShield() const
	{
		return m_enemyShield;
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
	int GetDamage() const
	{
		return m_getDamage;
	}

	struct EnemyStatus
	{
		int hp = 100000;				// 体力
		int attack = 10;			// 攻撃力
		int maxHp = 100000;			// 最大体力
	};

	const EnemyStatus& GetStatus() { return m_status; }


	// 回避成功フラグの取得
	bool GetJustAvoidSuccess() const { return m_justAvoidSuccess; }
	void SetJustAvoidSuccess(bool flag) { m_justAvoidSuccess = flag; }

	// OneceEffectの取得
	bool GetOnceEffect() const { return m_onceEffect; }

	void SetOnceEffect(bool flag) { m_onceEffect = flag; }

private:

	std::weak_ptr<Player> m_wpPlayer;

	bool m_Expired = false;				// 敵を消滅させるかどうか
	bool m_isHit = false;				// ヒット判定
	bool m_isAtkPlayer = false;

	int m_getDamage = 0;				// 受けるダメージ量

	float m_attackRadius = 1.5f;		// 攻撃判定の半径
	float m_attackFrame = 0.0f;			// 攻撃判定フレーム

	EnemyStatus m_status;				// 敵のステータス

	std::weak_ptr<EnemySword> m_enemySword; // 敵の剣
	std::weak_ptr<EnemyShield> m_enemyShield; // 敵の盾

	std::shared_ptr<HitDamage> m_spHitDamage;

	// ジャスト回避成功フラグ
	bool m_justAvoidSuccess = false;

	bool m_onceEffect = false;

};