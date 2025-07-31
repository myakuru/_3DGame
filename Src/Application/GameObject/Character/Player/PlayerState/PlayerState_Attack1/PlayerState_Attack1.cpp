#include "PlayerState_Attack1.h"
#include"../PlayerState_Attack2/PlayerState_Attack2.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_Run/PlayerState_Run.h"

void PlayerState_Attack1::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Attack1");
	m_player->GetAnimator()->AnimationBlend(anime, 10.0f, false);
	m_player->AnimeSetFlg() = true;
}

void PlayerState_Attack1::StateUpdate()
{
	m_player->SetAnimeSpeed(120.0f);

	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
	{
		auto attack1state = std::make_shared<PlayerState_Attack2>();
		m_player->ChangeState(attack1state);
		return;
	}

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto idleState = std::make_shared<PlayerState_Idle>();
		m_player->ChangeState(idleState);
		return;
	}
	else if (m_player->GetMoving() && m_player->GetAnimator()->IsAnimationEnd())
	{
		auto idleState = std::make_shared<PlayerState_Run>();
		m_player->ChangeState(idleState);
		return;
	}

	m_player->SetIsMoving(Math::Vector3::Zero);

}

void PlayerState_Attack1::StateEnd()
{
}
