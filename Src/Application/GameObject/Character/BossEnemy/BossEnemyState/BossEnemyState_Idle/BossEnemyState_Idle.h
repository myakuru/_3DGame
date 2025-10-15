#pragma once
#include"../BossEnemyState.h"
class EnemyShineBlue;
class BossEnemyState_Idle : public BossEnemyStateBase
{
public:
	BossEnemyState_Idle() = default;
	~BossEnemyState_Idle()override = default;

private:

	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;

	std::list<std::weak_ptr<EnemyShineBlue>> m_shineEffectBlues;

};