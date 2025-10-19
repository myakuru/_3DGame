#include "PlayerState_Sheathing-of-Katana.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_Attack/PlayerState_Attack.h"
#include"../PlayerState_BackWordAvoid/PlayerState_BackWordAvoid.h"
#include "../PlayerState_FowardAvoid/PlayerState_FowardAvoid.h"
#include"../../../../Weapon/Katana/Katana.h"
#include"../../../../Weapon/WeaponKatanaScabbard/WeaponKatanaScabbard.h"

void PlayerState_SheathKatana::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("SheathKatana");
	m_player->GetAnimator()->SetAnimation(anime, 0.9f, false);
	

	PlayerStateBase::StateStart();

	m_player->m_onceEffect = false;

	if(!m_SheathKatanaSound) m_SheathKatanaSound = KdAudioManager::Instance().Play("Asset/Sound/Player/SheathKatana.wav", false);
}

void PlayerState_SheathKatana::StateUpdate()
{
	m_player->SetAnimeSpeed(120.0f);

	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
	{
		auto attackState = std::make_shared<PlayerState_Attack>();
		m_player->ChangeState(attackState);
		return;
	}

	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_RBUTTON))
	{
		auto attackState = std::make_shared<PlayerState_ForwardAvoid>();
		m_player->ChangeState(attackState);
		return;
	}

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto idleState = std::make_shared<PlayerState_Idle>();
		m_player->ChangeState(idleState);
		return;
	}

	// カタナの取得
	auto katana = m_player->GetKatana().lock();
	if (!katana) return;
	
	float time = m_player->GetAnimator()->GetTime();

	katana->SetShowTrail(false);

	if (time >= 20.0f)
	{
		katana->SetNowAttackState(true);
		UpdateKatanaPos();
	}
	else
	{
		katana->SetNowAttackState(false);
		UpdateUnsheathed();
	}

}

void PlayerState_SheathKatana::StateEnd()
{
	PlayerStateBase::StateEnd();

	if(m_SheathKatanaSound)
	{
		m_SheathKatanaSound->Stop();
		m_SheathKatanaSound.reset();
	}
}
