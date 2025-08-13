#pragma once
#include"../PlayerState.h"
class PlayerState_Attack :public PlayerStateBase
{
public:
	PlayerState_Attack() = default;
	~PlayerState_Attack() override = default;

private:

	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;

	void UpdateKatanaPos();
	PlayerState_AttackParameter m_attackParam;
};