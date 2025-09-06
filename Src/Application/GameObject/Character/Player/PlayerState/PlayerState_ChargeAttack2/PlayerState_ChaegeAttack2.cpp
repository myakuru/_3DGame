#include "PlayerState_ChaegeAttack2.h"
#include"../../../../../main.h"
#include"../PlayerState_Attack1/PlayerState_Attack1.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_Run/PlayerState_Run.h"
#include"../../../../Weapon/Katana/Katana.h"

void PlayerState_ChaegeAttack2::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("ChargeAttack");
	m_player->GetAnimator()->AnimationBlend(anime, 10.0f, false);
	m_player->AnimeSetFlg() = true;

	PlayerStateBase::StateStart();

	m_attackParam = m_player->GetPlayerConfig().GetAttack2Param();
	m_attackParam.m_dashTimer = 0.0f;

	m_player->m_onceEffect = false;
}

void PlayerState_ChaegeAttack2::StateUpdate()
{
}

void PlayerState_ChaegeAttack2::StateEnd()
{
	PlayerStateBase::StateEnd();
}