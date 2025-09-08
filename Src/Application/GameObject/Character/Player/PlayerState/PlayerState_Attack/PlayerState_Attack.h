#pragma once
#include"../PlayerState.h"
class AttackEffect;
class PlayerState_Attack :public PlayerStateBase
{
public:
	PlayerState_Attack() = default;
	~PlayerState_Attack() override = default;

private:

	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;

	PlayerState_Attack2Parameter m_attackParam;

	std::weak_ptr<AttackEffect> m_attackEffect;

	bool m_once = false;
};