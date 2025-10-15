#pragma once
#include"../BossEnemyState.h"
class EnemyShineBlue;
class BossEnemyState_WaterAttack : public BossEnemyStateBase
{
public:
	BossEnemyState_WaterAttack() = default;
	~BossEnemyState_WaterAttack()override = default;

private:

	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;

	std::list<std::weak_ptr<EnemyShineBlue>> m_shineEffectBlues;

};