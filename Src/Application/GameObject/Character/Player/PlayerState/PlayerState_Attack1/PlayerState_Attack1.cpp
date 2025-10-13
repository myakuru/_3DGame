#include "PlayerState_Attack1.h"
#include"../PlayerState_Attack2/PlayerState_Attack2.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_Run/PlayerState_Run.h"
#include"../../../../../main.h"
#include"../../../../Weapon/Katana/Katana.h"
#include"../PlayerState_Sheathing-of-Katana/PlayerState_Sheathing-of-Katana.h"
#include"../../../../../Scene/SceneManager.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"

#include"../../../../Effect/EffekseerEffect/Rotation/Rotation.h"

#include"../PlayerState_BackWordAvoid/PlayerState_BackWordAvoid.h"
#include"../PlayerState_FowardAvoid/PlayerState_FowardAvoid.h"

void PlayerState_Attack1::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("newAttack2");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);
	

	PlayerStateBase::StateStart();

	m_attackParam = m_player->GetPlayerConfig().GetAttack1Param();

	m_attackParam.m_dashTimer = 0.0f;

	// 当たり判定リセット
	m_player->ResetAttackCollision();

	// 攻撃時はtrueにする
	if (auto katana = m_player->GetKatana().lock(); katana)
	{
		katana->SetNowAttackState(true);
	}

	m_effectOnce = false;

	m_LButtonkeyInput = false;

	// エフェクトの取得
	SceneManager::Instance().GetObjectWeakPtr(m_effect);

	m_player->SetAnimeSpeed(70.0f);
}

void PlayerState_Attack1::StateUpdate()
{
	// アニメーション時間のデバッグ表示
	{
		m_animeTime = m_player->GetAnimator()->GetPlayProgress();

		m_maxAnimeTime = m_player->GetAnimator()->GetMaxAnimationTime();

		if (m_animeTime > m_maxAnimeTime)
		{
			KdDebugGUI::Instance().AddLog(U8("Attack1アニメ時間: %f"), m_animeTime);
			KdDebugGUI::Instance().AddLog("\n");
		}
		else
		{
			m_animeTime = m_maxAnimeTime;
		}
	}

	float deltaTime = Application::Instance().GetDeltaTime();

	m_time += deltaTime;

	// 0.5秒間当たり判定有効
	m_player->UpdateAttackCollision(3.0f, 1.0f, 1, m_maxAnimeTime, { 0.2f, 0.2f }, 0.2f);
	
	
	// 回避入力受付
	{
		if (KeyboardManager::GetInstance().IsKeyPressed('W') && KeyboardManager::GetInstance().IsKeyJustPressed(VK_RBUTTON))
		{
			auto sheath = std::make_shared<PlayerState_BackWordAvoid>();
			m_player->ChangeState(sheath);
			return;
		}

		if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_RBUTTON))
		{
			auto sheath = std::make_shared<PlayerState_ForwardAvoid>();
			m_player->ChangeState(sheath);
			return;
		}
	}


	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
	{
		m_LButtonkeyInput = true;
	}

	// 攻撃中の移動方向で回転を更新
	if (m_player->GetMovement() != Math::Vector3::Zero)
	{
		Math::Vector3 moveDir = m_player->GetMovement();
		moveDir.y = 0.0f;
		moveDir.Normalize();
		m_player->UpdateQuaternionDirect(moveDir);
	}

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

		// エフェクトの再生
		if (auto effet = m_effect.lock(); effet)
		{
			effet->SetPlayEffect(true);
		}

		if (auto playreCamera = m_player->GetPlayerCamera().lock(); playreCamera)
		{
			playreCamera->StartShake({ m_player->GetCameraShakePower()}, m_player->GetCameraShakeTime());
		}

		// コンボ受付
		if (m_LButtonkeyInput)
		{
			// 80%以降で受付
			if (m_animeTime < 0.7f) return;
			auto next = std::make_shared<PlayerState_Attack2>();
			m_player->ChangeState(next);
			return;
		}
		else if (m_player->GetAnimator()->IsAnimationEnd())
		{
			auto sheath = std::make_shared<PlayerState_SheathKatana>();
			m_player->ChangeState(sheath);
			return;
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
