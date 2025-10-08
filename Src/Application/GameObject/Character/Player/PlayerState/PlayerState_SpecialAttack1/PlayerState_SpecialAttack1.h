#pragma once
#include"../PlayerState.h"
class SpecialAttack1;
class PlayerState_SpecialAttack1 :public PlayerStateBase
{
public:

	PlayerState_SpecialAttack1() = default;
	~PlayerState_SpecialAttack1() override = default;

private:

	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;

	PlayerState_Attack2Parameter m_attackParam;

	std::weak_ptr<SpecialAttack1> m_effect;

	bool m_effectOnce = false;

};