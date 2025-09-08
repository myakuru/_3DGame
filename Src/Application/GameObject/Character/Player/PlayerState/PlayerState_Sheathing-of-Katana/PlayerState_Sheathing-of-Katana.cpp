#include "PlayerState_Sheathing-of-Katana.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"

#include"../../../../Weapon/Katana/Katana.h"
#include"../../../../Weapon/WeaponKatanaScabbard/WeaponKatanaScabbard.h"

void PlayerState_SheathKatana::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("SheathKatana");
	m_player->GetAnimator()->AnimationBlend(anime, 10.0f, false);
	m_player->AnimeSetFlg() = true;

	PlayerStateBase::StateStart();

	m_player->m_onceEffect = false;
}

void PlayerState_SheathKatana::StateUpdate()
{
	m_player->SetAnimeSpeed(80.0f);

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto idleState = std::make_shared<PlayerState_Idle>();
		m_player->ChangeState(idleState);
		return;
	}

	// カタナの取得
	auto katana = m_player->GetKatana().lock();
	if (!katana) return;
	katana->SetNowAttackState(true);
	float time = m_player->GetAnimator()->GetTime();

	if (time <= 70.0f)
	{
		//m_player->SetAnimeSpeed(1.0f);
		katana->SetNowAttackState(true);
		UpdateKatanaPos();
	}
	else
	{
		//m_player->SetAnimeSpeed(0.05f);
		katana->SetNowAttackState(false);
		UpdateUnsheathed();
	}

}

void PlayerState_SheathKatana::StateEnd()
{
	PlayerStateBase::StateEnd();
}
