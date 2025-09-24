#include "PlayerState_SpecialAttackCutIn.h"
#include"../PlayerState_SpecialAttack/PlayerState_SpecialAttack.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"

void PlayerState_SpecialAttackCutIn::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("ChargeAttack0");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);
	m_player->AnimeSetFlg() = true;


	PlayerStateBase::StateStart();

	m_player->SetAnimeSpeed(20.0f);

	// カメラの位置を変更
	if (auto camera = m_player->GetPlayerCamera().lock(); camera)
	{
		camera->SetTargetLookAt({ 0.f,0.5f,-1.0f });
		camera->SetTargetRotation({ -10.0f,230.0f,0.0f });
	}

}

void PlayerState_SpecialAttackCutIn::StateUpdate()
{

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		// 必殺技状態へ
		auto state = std::make_shared<PlayerState_SpecialAttack>();
		m_player->ChangeState(state);
		return;
	}
}

void PlayerState_SpecialAttackCutIn::StateEnd()
{
	PlayerStateBase::StateEnd();
	// カメラの位置を変更
	if (auto camera = m_player->GetPlayerCamera().lock(); camera)
	{
		camera->SetTargetLookAt({ 0.f,1.f,-2.0f });
		camera->SetTargetRotation({ 0.0f,90.0f,0.0f });
	}
}
