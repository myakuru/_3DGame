#include "PlayerState_SpecialAttack.h"

#include"../../../../../main.h"
#include"../PlayerState_Sheathing-of-Katana/PlayerState_Sheathing-of-Katana.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"
#include"../../../../Weapon/Katana/Katana.h"

void PlayerState_SpecialAttack::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("PowerAttack");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);

	PlayerStateBase::StateStart();

	// 左手に刀をセットする
	if (auto katana = m_player->GetKatana().lock(); katana)
	{
		katana->SetNowAttackState(true);
	}
}

void PlayerState_SpecialAttack::StateUpdate()
{
	// アニメーション再生速度を変更
	m_player->SetAnimeSpeed(60.0f);

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto sheathState = std::make_shared<PlayerState_SheathKatana>();
		m_player->ChangeState(sheathState);
		return;
	}

	// 攻撃中の移動方向で回転を更新
	if (m_player->GetMovement() != Math::Vector3::Zero)
	{
		Math::Vector3 moveDir = m_player->GetMovement();
		moveDir.y = 0.0f;
		moveDir.Normalize();
		m_player->UpdateQuaternionDirect(moveDir);
	}

	m_player->SetIsMoving(m_attackDirection);

	UpdateKatanaPos();

	float deltaTime = Application::Instance().GetDeltaTime();

	if (m_time < 0.2f)
	{
		float dashSpeed = 0.0f;
		m_player->SetIsMoving(m_attackDirection * dashSpeed);
		m_time += deltaTime;
	}
	else
	{
		// 移動を止める
		m_player->SetIsMoving(Math::Vector3::Zero);
	}

	// カタナ関連
	if (auto katana = m_player->GetKatana().lock(); katana)
	{
		// 刀のトレイルポリゴンの表示
		katana->SetShowTrail(true);
	}
}

void PlayerState_SpecialAttack::StateEnd()
{
	PlayerStateBase::StateEnd();

	if (auto camera = m_player->GetPlayerCamera().lock(); camera)
	{
		// カメラと、プレイヤーの距離を近づける（らーぷ）
		camera->SetTargetLookAt({ 0.f,1.0f,-2.5f });
	}
}
