#include "PlayerState_SpecialAttackCutIn.h"
#include"../PlayerState_SpecialAttack/PlayerState_SpecialAttack.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"
#include"../../../../Weapon/Katana/Katana.h"

#include"../../../../../main.h"

void PlayerState_SpecialAttackCutIn::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("ChargeAttack0");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);
	


	PlayerStateBase::StateStart();

	m_player->SetAnimeSpeed(20.0f);

	// カメラの位置を変更
	if (auto camera = m_player->GetPlayerCamera().lock(); camera)
	{
		camera->SetTargetLookAt({ 0.f,0.5f,-1.0f });
		camera->SetTargetRotation({ -10.0f,230.0f,0.0f });
	}

	// 左手に刀をセットする
	if (auto katana = m_player->GetKatana().lock(); katana)
	{
		katana->SetNowAttackState(true);
	}

	m_time = 0.0f;

}

void PlayerState_SpecialAttackCutIn::StateUpdate()
{
	// 攻撃中の移動方向で回転を更新
	if (m_player->GetMovement() != Math::Vector3::Zero)
	{
		Math::Vector3 moveDir = m_player->GetMovement();
		moveDir.y = 0.0f;
		moveDir.Normalize();
		m_player->UpdateQuaternionDirect(moveDir);
	}

	m_player->SetIsMoving(m_attackDirection * 0.01f);

	UpdateKatanaPos();

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
		camera->SetTargetLookAt({ 0.f,1.f,-4.0f });
		camera->SetTargetRotation({ 0.0f,90.0f,0.0f });
	}
}
