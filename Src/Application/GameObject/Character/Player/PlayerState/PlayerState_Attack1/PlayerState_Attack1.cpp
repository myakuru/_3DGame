#include "PlayerState_Attack1.h"
#include"../PlayerState_Attack2/PlayerState_Attack2.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_Run/PlayerState_Run.h"
#include"../../../../../main.h"
#include"../../../../Weapon/Katana/Katana.h"
#include"../PlayerState_Sheathing-of-Katana/PlayerState_Sheathing-of-Katana.h"
#include"../../../../../Scene/SceneManager.h"
#include"../../../../Effect/EffekseerEffect/AttacEffect1/AttacEffect1.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"

void PlayerState_Attack1::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Attack1");
	m_player->GetAnimator()->SetAnimation(anime, 0.3f, false);
	

	PlayerStateBase::StateStart();

	m_attackParam = m_player->GetPlayerConfig().GetAttack1Param();

	m_attackParam.m_dashTimer = 0.0f;

	m_player->m_onceEffect = false;

	// 攻撃時はtrueにする
	if (auto katana = m_player->GetKatana().lock(); katana)
	{
		katana->SetNowAttackState(true);
	}

	m_effectOnce = false;

	m_LButtonkeyInput = false;

	// エフェクトの取得
	SceneManager::Instance().GetObjectWeakPtr(m_effect);
}

void PlayerState_Attack1::StateUpdate()
{

	// 0.5秒間当たり判定有効
	if (m_time <= 1.0 / 2)
	{
		m_player->UpdateAttack();
		m_time = 0.0f;
	}

	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
	{
		m_LButtonkeyInput = true;
	}

	// アニメ速度制御：予約があれば加速
	if (m_LButtonkeyInput)
	{
		m_player->SetAnimeSpeed(150.0f);
	}
	else
	{
		m_player->SetAnimeSpeed(100.0f);
	}

	// アニメ終了時の遷移
	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		if (m_LButtonkeyInput)
		{
			auto next = std::make_shared<PlayerState_Attack2>();
			m_player->ChangeState(next);
		}
		else
		{
			auto sheath = std::make_shared<PlayerState_SheathKatana>();
			m_player->ChangeState(sheath);
		}
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

	float deltaTime = Application::Instance().GetDeltaTime();
	if (m_attackParam.m_dashTimer < 0.2f)
	{
		float dashSpeed = 1.0f;
		m_player->SetIsMoving(m_attackDirection * dashSpeed);
		m_attackParam.m_dashTimer += deltaTime;
	}
	else
	{
		// 移動を止める
		m_player->SetIsMoving(Math::Vector3::Zero);

		if (auto playreCamera = m_player->GetPlayerCamera().lock(); playreCamera)
		{
			playreCamera->StartShake({ m_player->GetCameraShakePower()}, m_player->GetCameraShakeTime());
		}

		if (!m_effectOnce)
		{
			m_effectOnce = true;
			UpdateEffect();
		}
	}


	if (auto katana = m_player->GetKatana().lock(); katana)
	{
		// 剣の軌跡の表示
		katana->SetShowTrail(true);
		// 現在攻撃中フラグを立てる
		katana->SetNowAttackState(true);
		// 手と鞘の位置を更新
		UpdateKatanaPos();
	}

}

void PlayerState_Attack1::StateEnd()
{
	PlayerStateBase::StateEnd();

	// エフェクトがあったらフラグをfalseにする
	if(auto effect = m_effect.lock(); effect)
	{
		effect->SetPlayEffect(false);
	}

	// カタナの軌跡を消す
	if (auto katana = m_player->GetKatana().lock(); katana)
	{
		katana->SetNowAttackState(false);
	}
}

void PlayerState_Attack1::UpdateEffect()
{
	// エフェクトがあったらフラグをtrueにする
	if (auto effect = m_effect.lock(); effect)
	{
		effect->SetPlayEffect(true);
	}
}
