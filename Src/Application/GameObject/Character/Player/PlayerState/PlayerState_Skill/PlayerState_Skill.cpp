#include "PlayerState_Skill.h"
#include"../../../../../main.h"
#include"../PlayerState_Run/PlayerState_Run.h"
#include"../../../../Weapon/Katana/Katana.h"
#include"../PlayerState_Sheathing-of-Katana/PlayerState_Sheathing-of-Katana.h"

void PlayerState_Skill::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Eskill");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);
	

	PlayerStateBase::StateStart();

	m_attackParam.m_dashTimer = 0.0f;
}

void PlayerState_Skill::StateUpdate()
{
	m_player->SetAnimeSpeed(100.0f);

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto sheathState = std::make_shared<PlayerState_SheathKatana>();
		m_player->ChangeState(sheathState);
		return;
	}

	UpdateKatanaPos();

	float deltaTime = Application::Instance().GetDeltaTime();
	if (m_attackParam.m_dashTimer < 0.2f)
	{
		float dashSpeed = 1.0f;
		m_player->SetIsMoving(m_attackDirection * dashSpeed);
		m_attackParam.m_dashTimer += deltaTime;
	}
	else
	{
		// 移動を止める
		m_player->SetIsMoving(Math::Vector3::Zero);
	}

}

void PlayerState_Skill::StateEnd()
{
	PlayerStateBase::StateEnd();
}
