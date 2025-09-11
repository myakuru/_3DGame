#pragma once
#include"../PlayerState.h"
class PlayerState_Attack4 :public PlayerStateBase
{
public:
	PlayerState_Attack4() = default;
	~PlayerState_Attack4() override = default;

private:

	void StateStart() override;
	void StateUpdate() override;

	float m_time = 0.0f;

};