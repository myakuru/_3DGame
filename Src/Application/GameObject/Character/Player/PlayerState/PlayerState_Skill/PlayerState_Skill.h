#pragma once
#include"../PlayerState.h"
class PlayerState_Skill :public PlayerStateBase
{
public:
	PlayerState_Skill() = default;
	~PlayerState_Skill() override = default;

private:

	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;


	PlayerState_Attack2Parameter m_attackParam;


	bool m_flag = false; // 攻撃フラグ
};