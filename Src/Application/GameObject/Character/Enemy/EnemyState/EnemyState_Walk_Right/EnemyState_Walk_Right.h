#pragma once
#include"../EnemyStateBase.h"
class EnemyState_Walk_Right : public EnemyStateBase
{
public:
	EnemyState_Walk_Right() = default;
	~EnemyState_Walk_Right()override = default;

private:

	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;

	float m_orbitAngle = 0.0f;        // 現在角度（ラジアン）
	float m_orbitSpeed = 1.5f;        // 角速度（ラジアン/秒）調整可
	float m_orbitRadius = 5.5f;       // 目標半径

};