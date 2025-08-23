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
	PlayerState_Attack2Parameter m_attackParam;


	bool m_flag = false; // 攻撃フラグ
	float m_time = 0.0f;
};