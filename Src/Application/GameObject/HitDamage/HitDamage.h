#pragma once
#include "../Utility/SelectDraw2DTexture.h"
class Enemy;
class BossEnemy;
class PlayerCamera;
class HitDamage :public SelectDraw2DTexture
{
public:
	HitDamage() = default;
	~HitDamage() override = default;


	void Init() override;
	void Update() override;
	void DrawSprite() override;

	void SetDamage(int damage) { m_displayTime = damage; }

	void SetTrackEnemy(const std::weak_ptr<Enemy>& enemy) { m_enemy = enemy; }
	void SetTrackBossEnemy(const std::weak_ptr<BossEnemy>& bossEnemy) { m_bossEnemy = bossEnemy; }

private:

	std::list<std::weak_ptr<Enemy>> m_enemies;
	std::list<std::weak_ptr<BossEnemy>> m_bossEnemies; // 追加: BossEnemy リスト
	std::weak_ptr<PlayerCamera> m_camera;

	Math::Vector3 m_screenPos = Math::Vector3::Zero;
	Math::Vector3 m_offset = Math::Vector3::Zero;

	int m_displayTime = 0;

	float m_timer = 0.0f;

	Math::Vector3 m_enemyPos = Math::Vector3::Zero;

	std::weak_ptr<Enemy> m_enemy;
	std::weak_ptr<BossEnemy> m_bossEnemy;

	Math::Rectangle m_srcRect;
	int m_textureSizeX = 0;
	int m_textureSizeY = 0;

	bool m_foundHitEnemy = false;
};