#include "PlayerState_Attack4.h"
#include"../PlayerState_Sheathing-of-Katana/PlayerState_Sheathing-of-Katana.h"
#include"../../../../../main.h"
#include"../PlayerState_Attack1/PlayerState_Attack1.h"
#include"../../../../../Scene/SceneManager.h"

#include"../../../../Weapon/Katana/Katana.h"
#include"../../../../Effect/EffekseerEffect/GroundFreezes/GroundFreezes.h"
#include"../../../../Effect/EffekseerEffect/Rotation/Rotation.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"

#include"../PlayerState_BackWordAvoid/PlayerState_BackWordAvoid.h"
#include"../PlayerState_FowardAvoid/PlayerState_FowardAvoid.h"

void PlayerState_Attack4::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("newAttack5");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);

	PlayerStateBase::StateStart();

	// 攻撃時はtrueにする
	if (auto katana = m_player->GetKatana().lock(); katana)
	{
		katana->SetNowAttackState(true);
	}

	// 当たり判定リセット
	m_player->ResetAttackCollision();

	SceneManager::Instance().GetObjectWeakPtr(m_groundFreezes);
	SceneManager::Instance().GetObjectWeakPtr(m_rotation);

	m_time = 0.0f;
	m_LButtonkeyInput = false;

	m_player->SetAnimeSpeed(70.0f);

	if (m_player->GetPlayerStatus().chargeCount < 3)
	{
		m_player->GetPlayerStatus().chargeCount++;
	}
}

void PlayerState_Attack4::StateUpdate()
{
	// アニメーション時間のデバッグ表示
	{
		m_animeTime = m_player->GetAnimator()->GetPlayProgress();

		m_maxAnimeTime = m_player->GetAnimator()->GetMaxAnimationTime();

		if (m_animeTime > m_maxAnimeTime)
		{
			KdDebugGUI::Instance().AddLog(U8("Attack4アニメ時間: %f"), m_animeTime);
			KdDebugGUI::Instance().AddLog("\n");
		}
		else
		{
			m_animeTime = m_maxAnimeTime;
		}
	}

	PlayerStateBase::StateUpdate();

	float deltaTime = Application::Instance().GetDeltaTime();

	m_time += deltaTime;

	// 0.5秒間当たり判定有効
	m_player->UpdateAttackCollision(7.0f, 1.0f, 7, m_maxAnimeTime, { 0.2f, 0.2f }, 0.3f);

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

	UpdateKatanaPos();

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

		if (auto effect = m_groundFreezes.lock(); effect)
		{
			effect->SetPlayEffect(true);
		}

		if (auto effect = m_rotation.lock(); effect)
		{
			effect->SetPlayEffect(true);
		}

		// コンボ受付
		if (m_LButtonkeyInput)
		{
			// 90%以降で受付
			if (m_animeTime < 0.9f) return;
			auto next = std::make_shared<PlayerState_Attack1>();
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

void PlayerState_Attack4::StateEnd()
{
	PlayerStateBase::StateEnd();
	// カメラの位置を変更
	if (auto camera = m_player->GetPlayerCamera().lock(); camera)
	{
		camera->SetTargetLookAt({ 0.f,1.0f,-2.5f });
	}

	if (auto effect = m_groundFreezes.lock(); effect)
	{
		effect->SetPlayEffect(false);
		effect->StopEffect();
	}

	if (auto effect = m_rotation.lock(); effect)
	{
		effect->SetPlayEffect(false);
		effect->StopEffect();
	}

	m_player->SetIsMoving(Math::Vector3::Zero);
}
