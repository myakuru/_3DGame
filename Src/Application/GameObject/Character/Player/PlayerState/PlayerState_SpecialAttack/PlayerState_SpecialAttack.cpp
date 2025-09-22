#include "PlayerState_SpecialAttack.h"

#include"../../../../../main.h"
#include"../PlayerState_Sheathing-of-Katana/PlayerState_Sheathing-of-Katana.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"
#include"../../../../Weapon/Katana/Katana.h"

void PlayerState_SpecialAttack::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("PowerAttack");
	m_player->GetAnimator()->AnimationBlend(anime, 20.0f, false);

	PlayerStateBase::StateStart();

	// 左手に刀をセットする
	if (auto katana = m_player->GetKatana().lock(); katana)
	{
		katana->SetNowAttackState(true);
	}
}

void PlayerState_SpecialAttack::StateUpdate()
{
	//アニメーション再生速度を変更
	m_player->SetAnimeSpeed(60.0f);

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto sheathState = std::make_shared<PlayerState_SheathKatana>();
		m_player->ChangeState(sheathState);
		return;
	}

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
