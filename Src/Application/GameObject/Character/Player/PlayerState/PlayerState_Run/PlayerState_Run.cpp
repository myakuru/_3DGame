#include "PlayerState_Run.h"
#include"../../../CharacterBase.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"

void PlayerState_Run::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("RunFast");
	m_player->GetAnimator()->AnimationBlend(anime, 10.0f);
	m_player->AnimeSetFlg() = true;
}

void PlayerState_Run::StateUpdate()
{
	m_player->SetAnimeSpeed(60.0f);

	m_player->UpdateMoveDirectionFromInput();
	Math::Vector3 moveDir = m_player->GetMoveDirection();

	if (moveDir == Math::Vector3::Zero)
	{
		auto state = std::make_shared<PlayerState_Idle>();
		m_player->ChangeState(state);
		return;
	}

	if (!m_player->GetPlayerCamera()) return;

	m_player->UpdateQuaternion(moveDir);
	m_player->SetIsMoving(moveDir);

	KdDebugGUI::Instance().AddLog(std::to_string(moveDir.x).data());
	KdDebugGUI::Instance().AddLog(", ");
	KdDebugGUI::Instance().AddLog(std::to_string(moveDir.y).data());
	KdDebugGUI::Instance().AddLog(", ");
	KdDebugGUI::Instance().AddLog(std::to_string(moveDir.z).data());
	KdDebugGUI::Instance().AddLog("\n");
}

void PlayerState_Run::StateEnd()
{
}
