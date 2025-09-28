#include "PlayerState_RunStart.h"
#include"../PlayerState_Run/PlayerState_Run.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"

void PlayerState_RunStart::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("RunStart");
	m_player->GetAnimator()->SetAnimation(anime, 0.01f, false);

	m_player->SetAnimeSpeed(60.0f);
	PlayerStateBase::StateStart();

}

void PlayerState_RunStart::StateUpdate()
{
	// 移動方向を取得
	Math::Vector3 moveDir = m_player->GetMoveDirection();

	// キー入力から移動方向を更新
	m_player->UpdateMoveDirectionFromInput();

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto state = std::make_shared<PlayerState_Run>();
		m_player->ChangeState(state);
		return;
	}

	if (moveDir == Math::Vector3::Zero)
	{
		auto state = std::make_shared<PlayerState_Idle>();
		m_player->ChangeState(state);
		return;
	}

	UpdateUnsheathed();

	if (!m_player->GetPlayerCamera().lock()) return;

	m_player->UpdateQuaternion(moveDir);
	m_player->SetIsMoving(moveDir);
}

void PlayerState_RunStart::StateEnd()
{
	PlayerStateBase::StateEnd();
}
