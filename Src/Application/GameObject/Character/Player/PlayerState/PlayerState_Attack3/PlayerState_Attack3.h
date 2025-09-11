#pragma once
#include"../PlayerState.h"
class PlayerState_Attack3 :public PlayerStateBase
{
public:
	PlayerState_Attack3() = default;
	~PlayerState_Attack3() override = default;

private:

	void StateStart() override;
	void StateUpdate() override;

	float m_time = 0.0f;

};