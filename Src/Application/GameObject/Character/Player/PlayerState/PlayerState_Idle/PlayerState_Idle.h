﻿#pragma once
#include"../PlayerState.h"
class PlayerState_Idle :public PlayerStateBase
{
public:
	PlayerState_Idle() = default;
	~PlayerState_Idle() override = default;

private:

	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;

	void ImGuiInspector() override;

	void UpdateKatana();

	bool m_lButtonPressing = false;

};