#include "PlayerState_Attack2.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_Sheathing-of-Katana/PlayerState_Sheathing-of-Katana.h"
#include"../../../../../main.h"
#include"../../../../Weapon/Katana/Katana.h"

#include"../PlayerState_Attack3/PlayerState_Attack3.h"
#include"../../../../../Scene/SceneManager.h"
#include"../../../../Effect/EffekseerEffect/Rotation/Rotation.h"

void PlayerState_Attack2::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Attack2");
	m_player->GetAnimator()->SetAnimation(anime, 0.3f, false);
	PlayerStateBase::StateStart();

	m_attackParam = m_player->GetPlayerConfig().GetAttackParam();

	m_player->m_onceEffect = false;

	// 攻撃時はtrueにする
	if (auto katana = m_player->GetKatana().lock(); katana)
	{
		katana->SetNowAttackState(true);
	}

	m_keyInput = false;				// 次段コンボ予約フラグ初期化
}

void PlayerState_Attack2::StateUpdate()
{
	SceneManager::Instance().GetObjectWeakPtr(m_slashEffect);

	// 0.5秒間当たり判定有効
	if (m_time <= 1.0 / 2)
	{
		m_player->UpdateAttack();
		m_time = 0.0f;
	}

	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
	{
		m_keyInput = true;
	}

	// アニメ速度制御：予約があれば加速
	if (m_keyInput)
	{
		m_player->SetAnimeSpeed(130.0f);
	}
	else
	{
		m_player->SetAnimeSpeed(100.0f);
	}

	// アニメ終了時の遷移
	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		if (m_keyInput)
		{
			auto next = std::make_shared<PlayerState_Attack3>();
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

	float deltaTime = Application::Instance().GetDeltaTime();
	if (m_attackParam.m_dashTimer < 0.2f)
	{
		float dashSpeed = 0.5f;
		m_player->SetIsMoving(m_attackDirection * dashSpeed);
		m_attackParam.m_dashTimer += deltaTime;
	}
	else
	{
		if (auto effect = m_slashEffect.lock(); effect)
		{
			effect->SetPlayEffect(true);
		}

		// 移動を止める
		m_player->SetIsMoving(Math::Vector3::Zero);
	}
}

void PlayerState_Attack2::StateEnd()
{
	PlayerStateBase::StateEnd();

	if (auto effect = m_slashEffect.lock(); effect)
	{
		effect->SetPlayEffect(false);
	}
}
