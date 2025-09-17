#pragma once
#include"../PlayerState.h"
#include"../../../../Effect/FowardEffect/ForwardEffect.h"
class PlayerState_ChargeAttack :public PlayerStateBase
{
public:
	PlayerState_ChargeAttack() = default;
	~PlayerState_ChargeAttack() override = default;

private:

	void StateStart() override;
	void StateUpdate() override;
	void StateEnd() override;

	float m_chargeTime = 0.0f; // チャージ時間

	bool m_isCharging = false; // チャージ中かどうか
	bool m_isCharged = false; // チャージが完了したかどうか

	std::shared_ptr<ForwardEffect> m_forwardEffect = std::make_shared<ForwardEffect>();

	int m_chargeAttackCount = 0;      // 何回呼んだか

	bool m_flag = false; // 攻撃フラグ

};