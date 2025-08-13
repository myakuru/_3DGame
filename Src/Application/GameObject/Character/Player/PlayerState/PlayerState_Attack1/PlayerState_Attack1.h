#pragma once
#include"../PlayerState.h"
class PlayerState_Attack1 : public PlayerStateBase
{
public:
	PlayerState_Attack1() = default;
	~PlayerState_Attack1() override = default;

private:

	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;
	void UpdateKatanaPos();

	PlayerState_Attack1Parameter m_attackParam; // 攻撃パラメータ
};