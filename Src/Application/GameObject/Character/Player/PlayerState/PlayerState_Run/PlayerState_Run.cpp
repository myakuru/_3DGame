#include "PlayerState_Run.h"
#include"../../../CharacterBase.h"
#include"../PlayerState_RunEnd/PlayerState_RunEnd.h"
#include"../PlayerState_BackWordAvoid/PlayerState_BackWordAvoid.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"
#include"../PlayerState_Hit/PlayerState_Hit.h"
#include"../../../../Weapon/Katana/Katana.h"
#include"../../../../Weapon/WeaponKatanaScabbard/WeaponKatanaScabbard.h"

void PlayerState_Run::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Run");
	m_player->GetAnimator()->SetAnimation(anime);
	m_player->SetAnimeSpeed(70.0f);
}

void PlayerState_Run::StateUpdate()
{
	UpdateUnsheathed();

	m_player->UpdateMoveDirectionFromInput();
	Math::Vector3 moveDir = m_player->GetMoveDirection();

	if (moveDir == Math::Vector3::Zero)
	{
		auto state = std::make_shared<PlayerState_RunEnd>();
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

	if (!m_player->GetPlayerCamera().lock()) return;

	m_player->UpdateQuaternion(moveDir);
	m_player->SetIsMoving(moveDir);

}

void PlayerState_Run::StateEnd()
{
	PlayerStateBase::StateEnd();

	m_player->m_isHit = false; // ダメージフラグをリセット
}
