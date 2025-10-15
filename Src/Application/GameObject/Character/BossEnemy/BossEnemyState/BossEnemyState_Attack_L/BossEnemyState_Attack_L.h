#pragma once
#include"../BossEnemyState.h"
class EnemyShineBlue;
class BossEnemyState_Attack_L : public BossEnemyStateBase
{
public:
	BossEnemyState_Attack_L() = default;
	~BossEnemyState_Attack_L()override = default;

private:

	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;

	std::list<std::weak_ptr<EnemyShineBlue>> m_shineEffectBlues;

	int m_randomNum = 0;

};