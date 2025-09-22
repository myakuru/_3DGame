#include "PlayerState_Attack4.h"
#include"../PlayerState_Sheathing-of-Katana/PlayerState_Sheathing-of-Katana.h"
#include"../../../../../main.h"

#include"../../../../Weapon/Katana/Katana.h"

void PlayerState_Attack4::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Attack4");
	m_player->GetAnimator()->AnimationBlend(anime, 0.1f, false);
	m_player->AnimeSetFlg() = true;

	PlayerStateBase::StateStart();

	// 攻撃時はtrueにする
	if (auto katana = m_player->GetKatana().lock(); katana)
	{
		katana->SetNowAttackState(true);
	}

	m_time = 0.0f;
}

void PlayerState_Attack4::StateUpdate()
{
	PlayerStateBase::StateUpdate();

	float deltaTime = Application::Instance().GetDeltaTime();

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto state = std::make_shared<PlayerState_SheathKatana>();
		m_player->ChangeState(state);
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

	UpdateKatanaPos();

	if (m_time < 0.2f)
	{
		float dashSpeed = 0.7f;
		m_player->SetIsMoving(m_attackDirection * dashSpeed);
		m_time += deltaTime;
	}
	else
	{
		// 移動を止める
		m_player->SetIsMoving(Math::Vector3::Zero);
	}
}
