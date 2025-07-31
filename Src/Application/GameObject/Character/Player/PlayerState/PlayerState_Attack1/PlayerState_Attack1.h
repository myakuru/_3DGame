#pragma once
#include"../PlayerState.h"
class PlayerState_Attack1 : public PlayerStateBase
{
public:
	PlayerState_Attack1() = default;
	~PlayerState_Attack1() override = default;


	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;

};