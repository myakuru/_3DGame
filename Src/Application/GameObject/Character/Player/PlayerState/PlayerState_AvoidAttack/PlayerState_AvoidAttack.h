﻿#pragma once
#include"../PlayerState.h"
class PlayerState_AvoidAttack :public PlayerStateBase
{
public:
	PlayerState_AvoidAttack() = default;
	~PlayerState_AvoidAttack() override = default;

private:
	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;

	Math::Vector3 m_avoidDirection = Math::Vector3::Zero;
};