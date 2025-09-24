#pragma once
#include"../PlayerState.h"
class PlayerState_SheathKatana :public PlayerStateBase
{
public:
	PlayerState_SheathKatana() = default;
	~PlayerState_SheathKatana() override = default;

private:

	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;
};