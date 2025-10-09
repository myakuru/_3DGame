#include "PlayerState_AvoidAttack.h"
#include"../../../../../main.h"
#include"../PlayerState_Sheathing-of-Katana/PlayerState_Sheathing-of-Katana.h"
#include"../../../../../Scene/SceneManager.h"
#include"../../../../Effect/EffekseerEffect/SpeedAttackEffect/SpeedAttackEffect.h"

void PlayerState_AvoidAttack::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("AvoidAttack");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);
	PlayerStateBase::StateStart();
	m_time = 0.0f;

	// 当たり判定リセット
	m_player->ResetAttackCollision();

	SceneManager::Instance().GetObjectWeakPtr(m_effect);
}

void PlayerState_AvoidAttack::StateUpdate()
{

	m_player->UpdateAttackCollision(2.0f, 1.1f, 1, 0.3f, { 0.3f, 0.0f }, 0.3f);

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto runState = std::make_shared<PlayerState_SheathKatana>();
		m_player->ChangeState(runState);
		return;
	}
	// 回避中の移動方向で回転を更新
	if (m_player->GetMovement() != Math::Vector3::Zero)
	{
		Math::Vector3 moveDir = m_player->GetMovement();
		moveDir.y = 0.0f;
		moveDir.Normalize();
		m_player->UpdateQuaternionDirect(moveDir);
	}

	float deltaTime = Application::Instance().GetDeltaTime();

	m_time += deltaTime;

	if (m_time < 0.3f)
	{
		float dashSpeed = 2.0f;
		m_player->SetIsMoving(m_attackDirection * dashSpeed);
		m_time += deltaTime;
	}
	else
	{
		// 移動を止める
		m_player->SetIsMoving(Math::Vector3::Zero);

		if (auto effect = m_effect.lock(); effect)
		{
			effect->SetPlayEffect(true);
		}
	}
	PlayerStateBase::StateUpdate();
}

void PlayerState_AvoidAttack::StateEnd()
{
	PlayerStateBase::StateEnd();

	if (auto effect = m_effect.lock(); effect)
	{
		effect->SetPlayEffect(true);
		effect->StopEffect();
	}
}
