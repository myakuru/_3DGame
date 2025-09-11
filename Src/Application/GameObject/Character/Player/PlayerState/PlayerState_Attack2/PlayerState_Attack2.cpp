#include "PlayerState_Attack2.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_Run/PlayerState_Run.h"
#include"../../../../../main.h"
#include"../../../../Weapon/Katana/Katana.h"

#include"../PlayerState_Attack3/PlayerState_Attack3.h"

void PlayerState_Attack2::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Attack2");
	m_player->GetAnimator()->AnimationBlend(anime, 10.0f, false);
	m_player->AnimeSetFlg() = true;

	PlayerStateBase::StateStart();

	m_attackParam = m_player->GetPlayerConfig().GetAttackParam();

	m_player->m_onceEffect = false;

	m_flag = false; // 攻撃フラグ

	// カタナの取得
	auto katana = m_player->GetKatana().lock();

	if (!katana) return;

	katana->SetNowAttackState(true);

}

void PlayerState_Attack2::StateUpdate()
{
	m_player->SetAnimeSpeed(120.0f);

	// 0.5秒間当たり判定有効
	if (m_time <= 1.0 / 2)
	{
		m_player->UpdateAttack();
		m_time = 0.0f;
	}

	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
	{
		auto attack1state = std::make_shared<PlayerState_Attack3>();
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

	UpdateKatanaPos();

	// 攻撃中の移動方向で回転を更新
	if (m_player->GetMovement() != Math::Vector3::Zero)
	{
		Math::Vector3 moveDir = m_player->GetMovement();
		moveDir.y = 0.0f;
		moveDir.Normalize();
		m_player->UpdateQuaternionDirect(moveDir);
	}

	float deltaTime = Application::Instance().GetDeltaTime();
	if (m_attackParam.m_dashTimer < 0.2f)
	{
		float dashSpeed = 0.5f;
		m_player->SetIsMoving(m_attackDirection * dashSpeed);
		m_attackParam.m_dashTimer += deltaTime;
	}
	else
	{
		// 移動を止める
		m_player->SetIsMoving(Math::Vector3::Zero);
	}
}

void PlayerState_Attack2::StateEnd()
{
	m_player->AnimeSetFlg() = false;
	PlayerStateBase::StateEnd();
}
