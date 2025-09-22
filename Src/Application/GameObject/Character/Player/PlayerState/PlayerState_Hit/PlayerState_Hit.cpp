#include "PlayerState_Hit.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_Attack/PlayerState_Attack.h"

#include"../../../../Weapon/Katana/Katana.h"

void PlayerState_Hit::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Hit");
	m_player->GetAnimator()->AnimationBlend(anime, 10.0f, false);
	m_player->AnimeSetFlg() = true;

	m_player->m_onceEffect = false;
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

	m_player->SetIsMoving(Math::Vector3::Zero);
}

void PlayerState_Hit::StateEnd()
{
	PlayerStateBase::StateEnd();
}
