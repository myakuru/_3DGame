#pragma once
#include"../PlayerState.h"
class PlayerState_ChaegeAttack2 :public PlayerStateBase
{
public:
	PlayerState_ChaegeAttack2() = default;
	~PlayerState_ChaegeAttack2() override = default;

private:

	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;

	void UpdateKatanaPos();
	PlayerState_Attack2Parameter m_attackParam;


	bool m_flag = false; // 攻撃フラグ
};