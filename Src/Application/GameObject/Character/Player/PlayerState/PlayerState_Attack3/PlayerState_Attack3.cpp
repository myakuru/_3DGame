#include "PlayerState_Attack3.h"
#include"../PlayerState_Sheathing-of-Katana/PlayerState_Sheathing-of-Katana.h"
#include"../../../../../main.h"
#include"../PlayerState_Attack4/PlayerState_Attack4.h"
#include"../../../../../Scene/SceneManager.h"
#include"../../../../Effect/EffekseerEffect/SlashEffect/SlashEffect.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"
	
#include"../../../../Weapon/Katana/Katana.h"

void PlayerState_Attack3::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Attack3");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);

	PlayerStateBase::StateStart();

	// 攻撃時はtrueにする
	if (auto katana = m_player->GetKatana().lock(); katana)
	{
		katana->SetNowAttackState(true);
	}

	m_player->m_onceEffect = false;

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
		camera->SetTargetLookAt({ 0.f,1.0f,-5.0f });
	}
}

void PlayerState_Attack3::StateUpdate()
{

	PlayerStateBase::StateUpdate();

	float deltaTime = Application::Instance().GetDeltaTime();

	m_time += deltaTime;

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

	// アニメ速度制御
	if (m_LButtonkeyInput)
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
		if (m_LButtonkeyInput)
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
