#pragma once
#include"../PlayerState.h"
class PlayerState_SpecialAttack :public PlayerStateBase
{
public:
	PlayerState_SpecialAttack() = default;
	~PlayerState_SpecialAttack() override = default;

private:

	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;

};