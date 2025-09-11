#include "PlayerState_Attack3.h"
#include"../PlayerState_Sheathing-of-Katana/PlayerState_Sheathing-of-Katana.h"
#include"../../../../../main.h"
#include"../PlayerState_Attack4/PlayerState_Attack4.h"

#include"../../../../Weapon/Katana/Katana.h"

void PlayerState_Attack3::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Attack3");
	m_player->GetAnimator()->AnimationBlend(anime, 0.1f, false);
	m_player->AnimeSetFlg() = true;

	PlayerStateBase::StateStart();

	// カタナの取得
	auto katana = m_player->GetKatana().lock();

	if (!katana) return;

	katana->SetNowAttackState(true);

	m_time = 0.0f;
}

void PlayerState_Attack3::StateUpdate()
{
	PlayerStateBase::StateUpdate();

	float deltaTime = Application::Instance().GetDeltaTime();

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto state = std::make_shared<PlayerState_SheathKatana>();
		m_player->ChangeState(state);
		return;
	}

	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
	{
		auto attack1state = std::make_shared<PlayerState_Attack4>();
		m_player->ChangeState(attack1state);
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
