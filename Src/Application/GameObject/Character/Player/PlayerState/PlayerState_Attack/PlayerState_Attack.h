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

private:
	Math::Vector3 prevRootTranslation = Math::Vector3::Zero;
	Math::Vector3 currentRootTranslation = Math::Vector3::Zero;
	Math::Vector3 m_attackDirection = Math::Vector3::Zero;
};