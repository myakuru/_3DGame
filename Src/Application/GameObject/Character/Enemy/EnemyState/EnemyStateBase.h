#pragma once
#include"../../../../../MyFramework/State/StateBase/StateBase.h"
#include"../Enemy.h"
class EnemyStateBase : public StateBase
{
public:
	EnemyStateBase() = default;
	~EnemyStateBase() override = default;

	void SetEnemy(Enemy* player) { m_enemy = player; }

protected:
	Enemy* m_enemy = nullptr;

};