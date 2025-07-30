#pragma once
#include"../PlayerState.h"
class PlayerState_Attack :public PlayerStateBase
{
public:
	PlayerState_Attack() = default;
	~PlayerState_Attack() override = default;

	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;

};