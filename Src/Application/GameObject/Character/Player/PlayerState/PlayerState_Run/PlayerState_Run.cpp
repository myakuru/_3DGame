#include "PlayerState_Run.h"
#include"../../../CharacterBase.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_BackWordAvoid/PlayerState_BackWordAvoid.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"
#include"../PlayerState_Hit/PlayerState_Hit.h"
#include"../../../../Weapon/Katana/Katana.h"
#include"../../../../Weapon/WeaponKatanaScabbard/WeaponKatanaScabbard.h"

void PlayerState_Run::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Run");
	m_player->GetAnimator()->SetAnimation(anime);
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

	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_RBUTTON))
	{
		auto state = std::make_shared<PlayerState_BackWordAvoid>();
		m_player->ChangeState(state);
		return; 
	}

	PlayerStateBase::StateUpdate();

	// ダメージを受けたらHitステートへ
	if (m_player->m_isHit)
	{
		auto spHitState = std::make_shared<PlayerState_Hit>();
		m_player->ChangeState(spHitState);
		return;
	}

	UpdateUnsheathed();

	if (!m_player->GetPlayerCamera().lock()) return;

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
	PlayerStateBase::StateEnd();

	m_player->m_isHit = false; // ダメージフラグをリセット
}
