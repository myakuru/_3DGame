#include "PlayerState_AvoidAttack.h"
#include"../../../../../main.h"
#include"../PlayerState_Run/PlayerState_Run.h"

void PlayerState_AvoidAttack::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("AvoidAttack");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);
	PlayerStateBase::StateStart();
}

void PlayerState_AvoidAttack::StateUpdate()
{
	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto runState = std::make_shared<PlayerState_Run>();
		m_player->ChangeState(runState);
		return;
	}
	// 回避中の移動方向で回転を更新
	if (m_player->GetMovement() != Math::Vector3::Zero)
	{
		Math::Vector3 moveDir = m_player->GetMovement();
		moveDir.y = 0.0f;
		moveDir.Normalize();
		m_player->UpdateQuaternionDirect(moveDir);
	}
	m_player->SetIsMoving(m_avoidDirection);

	float deltaTime = Application::Instance().GetDeltaTime();
	if (m_time < 0.3f)
	{
		float dashSpeed = 1.0f;
		m_player->SetIsMoving(m_avoidDirection * dashSpeed);
		m_time += deltaTime;
	}
	else
	{
		// 移動を止める
		m_player->SetIsMoving(Math::Vector3::Zero);
	}
	PlayerStateBase::StateUpdate();
}

void PlayerState_AvoidAttack::StateEnd()
{
	PlayerStateBase::StateEnd();
}
