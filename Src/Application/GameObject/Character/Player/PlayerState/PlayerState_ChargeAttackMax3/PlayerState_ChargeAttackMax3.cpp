#include "PlayerState_ChargeAttackMax3.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"

void PlayerState_ChargeAttackMax3::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("ChargeMax");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);
	PlayerStateBase::StateStart();
	// アニメーション速度を変更
	m_player->SetAnimeSpeed(60.0f);
}

void PlayerState_ChargeAttackMax3::StateUpdate()
{
	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto state = std::make_shared<PlayerState_Idle>();
		m_player->ChangeState(state);
		return;
	}

	// 刀は鞘の中にある状態
	UpdateUnsheathed();

	PlayerStateBase::StateUpdate();

	// 攻撃中の移動方向で回転を更新
	if (m_player->GetMovement() != Math::Vector3::Zero)
	{
		Math::Vector3 moveDir = m_player->GetMovement();
		moveDir.y = 0.0f;
		moveDir.Normalize();
		m_player->UpdateQuaternionDirect(moveDir);
	}

	m_player->SetIsMoving(Math::Vector3::Zero);
}

void PlayerState_ChargeAttackMax3::StateEnd()
{
	PlayerStateBase::StateEnd();
	if (auto camera = m_player->GetPlayerCamera().lock(); camera)
	{
		camera->SetTargetLookAt(Math::Vector3(0.0f, 1.0f, -3.5f));
		camera->SetDistanceSmooth(8.f);
		camera->SetRotationSmooth(8.f);
	}
}
