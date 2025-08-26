#pragma once
#include "../../Utility/SelectDraw2DTexture.h"
class Enemy;
class PlayerCamera;
class EnemyHp : public SelectDraw2DTexture
{
public:
	EnemyHp() = default;
	~EnemyHp() override = default;

	void Init() override;
	void Update() override;
	void DrawSprite() override;

private:

	std::weak_ptr<Enemy> m_enemy;
	std::weak_ptr<PlayerCamera> m_camera;

	Math::Vector3 m_screenPos = Math::Vector3::Zero;
	Math::Vector3 m_offset = Math::Vector3::Zero;

	int m_displayTime = 0;

	float m_timer = 0.0f;

};