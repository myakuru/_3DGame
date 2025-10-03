#pragma once
#include"../../../../../MyFramework/State/StateBase/StateBase.h"
#include"../Enemy.h"
#include"../../../../main.h"
class EnemyStateBase : public StateBase
{
public:
	EnemyStateBase() = default;
	~EnemyStateBase() override = default;

	void SetEnemy(Enemy* player) { m_enemy = player; }

protected:

	void StateStart() override = 0;
	void StateUpdate() override = 0;
	void StateEnd() override = 0;


	Math::Vector3 m_attackDirection = Math::Vector3::Zero;

	Enemy* m_enemy = nullptr;
	float m_time = 0.0f;

};