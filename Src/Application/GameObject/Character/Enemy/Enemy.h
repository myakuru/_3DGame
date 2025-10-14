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

	// 攻撃の当たり判定(攻撃半径、攻撃距離、攻撃回数、攻撃間隔)
	void UpdateAttackCollision(float _radius = 1.f, float _distance = 1.1f,
		int _attackCount = 5, float _attackTimer = 0.3f);

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
		int hp = 1000000000;				// 体力
		int attack = 10;			// 攻撃力
		int maxHp = 1000000000;			// 最大体力
	};

	const EnemyStatus& GetStatus() { return m_status; }


	// 回避成功フラグの取得
	bool GetJustAvoidSuccess() const { return m_justAvoidSuccess; }
	void SetJustAvoidSuccess(bool flag) { m_justAvoidSuccess = flag; }

	// 当たり判定リセット
	void ResetAttackCollision()
	{
		m_chargeAttackCount = 0;
		m_chargeAttackTimer = 0.0f;
		m_isChargeAttackActive = false;
		m_hitOnce = false;
	}

	// 無敵状態管理フラグ
	void SetInvincible(bool _flag) { m_invincible = _flag; }

	bool GetInvincible() const { return m_invincible; }

	// 敵への累積ヒット回数（全ステート共通）
	int  GetHitCount() const { return m_totalHitCount; }
	void IncrementHitCount() { ++m_totalHitCount; }
	void ResetHitCount() { m_totalHitCount = 0; }

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

	std::list<std::weak_ptr<Player>> m_player;

	// ジャスト回避成功フラグ
	bool m_justAvoidSuccess = false;

	bool m_hitOnce = false;
	int m_chargeAttackCount = 0;								// 何回ダメージを与えたか
	float m_chargeAttackTimer = 0.0f;							// 経過時間
	bool m_isChargeAttackActive = false;						// 連続攻撃中か

	bool m_invincible = false;					// 無敵判定用
	int m_totalHitCount = 0;					// 累積ヒット回数（無敵判定用）
};