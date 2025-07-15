#pragma once
#include"../../../../../MyFramework/State/StateBase/StateBase.h"
#include"../Player.h"
class PlayerStateBase : public StateBase
{
public:
	PlayerStateBase() = default;
	~PlayerStateBase() override = default;

	void SetPlayer(Player* player) { m_player = player; }

private:
	Player* m_player = nullptr;

};