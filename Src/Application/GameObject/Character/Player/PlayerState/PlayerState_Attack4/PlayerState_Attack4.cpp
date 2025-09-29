#include "PlayerState_Attack4.h"
#include"../PlayerState_Sheathing-of-Katana/PlayerState_Sheathing-of-Katana.h"
#include"../../../../../main.h"
#include"../PlayerState_Attack1/PlayerState_Attack1.h"
#include"../../../../../Scene/SceneManager.h"

#include"../../../../Weapon/Katana/Katana.h"
#include"../../../../Effect/EffekseerEffect/GroundFreezes/GroundFreezes.h"
#include"../../../../Effect/EffekseerEffect/Rotation/Rotation.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"

void PlayerState_Attack4::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Attack4");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);

	PlayerStateBase::StateStart();

	// 攻撃時はtrueにする
	if (auto katana = m_player->GetKatana().lock(); katana)
	{
		katana->SetNowAttackState(true);
	}

	m_time = 0.0f;
	m_keyInput = false;

	SceneManager::Instance().GetObjectWeakPtr(m_groundFreezes);
	SceneManager::Instance().GetObjectWeakPtr(m_rotation);
}

void PlayerState_Attack4::StateUpdate()
{
	PlayerStateBase::StateUpdate();


	float deltaTime = Application::Instance().GetDeltaTime();

	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
	{
		m_keyInput = true;
	}

	if (m_keyInput)
	{
		m_player->SetAnimeSpeed(130.0f);
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
			auto next = std::make_shared<PlayerState_Attack1>();
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

	UpdateKatanaPos();

	if (m_time < 0.2f)
	{
		float dashSpeed = 1.0f;
		m_player->SetIsMoving(m_attackDirection * dashSpeed);
		m_time += deltaTime;
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
	}
}

void PlayerState_Attack4::StateEnd()
{
	PlayerStateBase::StateEnd();
	// カメラの位置を変更
	if (auto camera = m_player->GetPlayerCamera().lock(); camera)
	{
		camera->SetTargetLookAt({ 0.f,1.0f,-3.5f });
	}
}
