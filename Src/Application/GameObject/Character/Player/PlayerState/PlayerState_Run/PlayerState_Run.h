#pragma once
#include"../PlayerState.h"
class PlayerState_Run :public PlayerStateBase
{
public:
	PlayerState_Run() = default;
	~PlayerState_Run() override = default;

	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;
	void UpdateKatanaPos();

};