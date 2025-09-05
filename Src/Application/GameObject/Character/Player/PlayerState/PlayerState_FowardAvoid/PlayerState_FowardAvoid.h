#pragma once
#include"../PlayerState.h"
class PlayerState_ForwardAvoid :public PlayerStateBase
{
public:
	PlayerState_ForwardAvoid() = default;
	~PlayerState_ForwardAvoid() override = default;

private:

	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;

	float m_chargeTime = 0.0f; // チャージ時間

	bool m_isCharging = false; // チャージ中かどうか
	bool m_isCharged = false; // チャージが完了したかどうか

};