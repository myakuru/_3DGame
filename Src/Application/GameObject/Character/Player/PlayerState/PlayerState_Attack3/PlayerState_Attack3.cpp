#include "PlayerState_Attack3.h"
#include"../PlayerState_Sheathing-of-Katana/PlayerState_Sheathing-of-Katana.h"
#include"../../../../../main.h"
#include"../PlayerState_Attack4/PlayerState_Attack4.h"
#include"../../../../../Scene/SceneManager.h"
#include"../../../../Effect/EffekseerEffect/SlashEffect/SlashEffect.h"

#include"../../../../Weapon/Katana/Katana.h"

void PlayerState_Attack3::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Attack3");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);
	m_player->AnimeSetFlg() = true;

	PlayerStateBase::StateStart();

	// 攻撃時はtrueにする
	if (auto katana = m_player->GetKatana().lock(); katana)
	{
		katana->SetNowAttackState(true);
	}

	m_time = 0.0f;
	m_keyInput = false;
}

void PlayerState_Attack3::StateUpdate()
{
	SceneManager::Instance().GetObjectWeakPtr(m_slashEffect);

	if (auto effect = m_slashEffect.lock(); effect)
	{
		effect->SetPlayEffect(true);
	}

	PlayerStateBase::StateUpdate();

	float deltaTime = Application::Instance().GetDeltaTime();

	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
	{
		m_keyInput = true;
	}

	// アニメ速度制御：予約があれば加速
	if (m_keyInput)
	{
		m_player->SetAnimeSpeed(100.0f);
	}
	else
	{
		m_player->SetAnimeSpeed(80.0f);
	}

	// アニメ終了時の遷移
	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		if (m_keyInput)
		{
			auto next = std::make_shared<PlayerState_Attack4>();
			m_player->ChangeState(next);
		}
		else
		{
			auto sheath = std::make_shared<PlayerState_SheathKatana>();
			m_player->ChangeState(sheath);
		}
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
		float dashSpeed = 0.3f;
		m_player->SetIsMoving(m_attackDirection * dashSpeed);
		m_time += deltaTime;
	}
	else
	{
		// 移動を止める
		m_player->SetIsMoving(Math::Vector3::Zero);
	}

}

void PlayerState_Attack3::StateEnd()
{
	PlayerStateBase::StateEnd();

	// エフェクトの停止
	if (auto effect = m_slashEffect.lock(); effect)
	{
		effect->SetPlayEffect(false);
	}
}
