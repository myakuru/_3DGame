#pragma once
#include"../PlayerState.h"
class PlayerState_Attack2 : public PlayerStateBase
{
public:
	PlayerState_Attack2() = default;
	~PlayerState_Attack2() override = default;

private:

	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;

	void UpdateKatanaPos();

	PlayerState_Attack2Parameter m_attackParam; // 攻撃パラメータ

};