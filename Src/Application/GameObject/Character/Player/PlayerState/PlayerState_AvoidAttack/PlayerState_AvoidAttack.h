#pragma once
#include"../PlayerState.h"
class PlayerState_AvoidAttack :public PlayerStateBase
{
public:
	PlayerState_AvoidAttack() = default;
	~PlayerState_AvoidAttack() override = default;

	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;

private:

};