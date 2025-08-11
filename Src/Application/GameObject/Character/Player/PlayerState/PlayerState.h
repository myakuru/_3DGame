#pragma once
#include"../../../../../MyFramework/State/StateBase/StateBase.h"
#include"../Player.h"
class PlayerStateBase : public StateBase
{
public:
	PlayerStateBase() = default;
	~PlayerStateBase() override = default;

	void SetPlayer(Player* player) { m_player = player; }

protected:
	Player* m_player = nullptr;

	Math::Vector3 prevRootTranslation = Math::Vector3::Zero;
	Math::Vector3 currentRootTranslation = Math::Vector3::Zero;
	Math::Vector3 m_attackDirection = Math::Vector3::Zero;
	float m_dashTimer = 0.0f; // ダッシュのタイマー

};