#include "PlayerState_Attack3.h"
#include"../PlayerState_Sheathing-of-Katana/PlayerState_Sheathing-of-Katana.h"
#include"../../../../../main.h"
#include"../PlayerState_Attack4/PlayerState_Attack4.h"
#include"../../../../../Scene/SceneManager.h"
#include"../../../../Effect/EffekseerEffect/SlashEffect/SlashEffect.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"

#include"../PlayerState_BackWordAvoid/PlayerState_BackWordAvoid.h"
#include"../PlayerState_FowardAvoid/PlayerState_FowardAvoid.h"
	
#include"../../../../Weapon/Katana/Katana.h"

void PlayerState_Attack3::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("newAttack4");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);

	PlayerStateBase::StateStart();

	// 攻撃時はtrueにする
	if (auto katana = m_player->GetKatana().lock(); katana)
	{
		katana->SetNowAttackState(true);
	}

	// 当たり判定リセット
	m_player->ResetAttackCollision();

	m_time = 0.0f;
	m_LButtonkeyInput = false;

	SceneManager::Instance().GetObjectWeakPtr(m_slashEffect);

	if (auto effect = m_slashEffect.lock(); effect)
	{
		effect->SetPlayEffect(true);
	}

	// カメラの位置を変更
	if (auto camera = m_player->GetPlayerCamera().lock(); camera)
	{
		camera->SetTargetLookAt({ 0.f,1.0f,-4.5f });
	}

	// 残像の設定
	m_player->AddAfterImage(true, 3, 1, Math::Color(0.0f, 1.0f, 1.0f, 1.0f), 0.7f);

	m_player->SetAnimeSpeed(80.0f);
}

void PlayerState_Attack3::StateUpdate()
{
	// アニメーション時間のデバッグ表示
	{
		m_animeTime = m_player->GetAnimator()->GetPlayProgress();

		m_maxAnimeTime = m_player->GetAnimator()->GetMaxAnimationTime();

		if (m_animeTime > m_maxAnimeTime)
		{
			KdDebugGUI::Instance().AddLog(U8("Attack3アニメ時間: %f"), m_animeTime);
			KdDebugGUI::Instance().AddLog("\n");
		}
		else
		{
			m_animeTime = m_maxAnimeTime;
		}
	}

	if (m_animeTime >= 0.1f && m_animeTime <= 0.2f)
	{
		KdShaderManager::Instance().m_postProcessShader.SetEnableStrongBlur(true);
	}
	else if(m_animeTime >= 0.5f && m_animeTime <= 0.6f)
	{
		KdShaderManager::Instance().m_postProcessShader.SetEnableStrongBlur(true);
	}
	else if(m_animeTime >= 0.8f && m_animeTime <= 0.9f)
	{
		KdShaderManager::Instance().m_postProcessShader.SetEnableStrongBlur(true);
	}
	else
	{
		KdShaderManager::Instance().m_postProcessShader.SetEnableStrongBlur(false);
	}

	PlayerStateBase::StateUpdate();

	float deltaTime = Application::Instance().GetDeltaTime();

	m_time += deltaTime;

	// 0.5秒間当たり判定有効
	
	m_player->UpdateAttackCollision(10.0f, 1.1f, 5, 0.5f, { 0.3f, 0.0f }, 0.3f);

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

	// コンボ受付
	{
		if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
		{
			m_LButtonkeyInput = true;
		}
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

	if (m_attackParam.m_dashTimer < 0.2f)
	{
		float dashSpeed = 0.7f;
		m_player->SetIsMoving(m_attackDirection * dashSpeed);
		m_attackParam.m_dashTimer += deltaTime;
	}
	else
	{
		// 移動を止める
		m_player->SetIsMoving(Math::Vector3::Zero);

		// コンボ受付
		if (m_LButtonkeyInput)
		{
			// 70%以降で受付
			if (m_animeTime < 0.7f) return;
			auto next = std::make_shared<PlayerState_Attack4>();
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

}

void PlayerState_Attack3::StateEnd()
{
	PlayerStateBase::StateEnd();

	// エフェクトの停止
	if (auto effect = m_slashEffect.lock(); effect)
	{
		effect->SetPlayEffect(false);
		effect->StopEffect();
	}

	m_player->AddAfterImage();
}
