#include "PlayerState_Hit.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_Attack/PlayerState_Attack.h"

#include"../../../../Weapon/Katana/Katana.h"
#include"../../../../../main.h"

#include"../../../Enemy/Enemy.h"

void PlayerState_Hit::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Hit");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);

	PlayerStateBase::StateStart();

	m_time = 0.0f;
}

void PlayerState_Hit::StateUpdate()
{
	m_player->SetAnimeSpeed(120.0f);

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto idleState = std::make_shared<PlayerState_Idle>();
		m_player->ChangeState(idleState);
		return;
	}

	PlayerStateBase::StateUpdate();

	UpdateKatanaPos();

	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
	{
		auto attack1state = std::make_shared<PlayerState_Attack>();
		m_player->ChangeState(attack1state);
		return;
	}



	float deltaTime = Application::Instance().GetDeltaTime();

	if (m_time < 0.2f)
	{
		float dashSpeed = -0.5f;
		m_player->SetIsMoving(m_attackDirection * dashSpeed);
		m_time += deltaTime;
	}
	else
	{
		m_player->SetIsMoving(Math::Vector3::Zero);
	}
}

void PlayerState_Hit::StateEnd()
{
	PlayerStateBase::StateEnd();
	m_player->SetHitCheck(false);
}
