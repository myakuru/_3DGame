﻿#pragma once
#include"../PlayerState.h"
class PlayerState_Idle :public PlayerStateBase
{
public:
	PlayerState_Idle() = default;
	~PlayerState_Idle() override = default;

	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;

private:

};