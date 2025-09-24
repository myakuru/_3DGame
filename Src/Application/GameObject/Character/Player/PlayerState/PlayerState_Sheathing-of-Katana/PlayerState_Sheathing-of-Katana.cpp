#include "PlayerState_Sheathing-of-Katana.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_Attack/PlayerState_Attack.h"

#include"../../../../Weapon/Katana/Katana.h"
#include"../../../../Weapon/WeaponKatanaScabbard/WeaponKatanaScabbard.h"

void PlayerState_SheathKatana::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("SheathKatana");
	m_player->GetAnimator()->SetAnimation(anime, 0.9f, false);
	m_player->AnimeSetFlg() = true;

	PlayerStateBase::StateStart();

	m_player->m_onceEffect = false;
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

	//KdDebugGUI::Instance().AddLog(std::to_string(time).data());
	//KdDebugGUI::Instance().AddLog("\n");

	katana->SetShowTrail(false);

	if (time >= 20.f)
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
}
