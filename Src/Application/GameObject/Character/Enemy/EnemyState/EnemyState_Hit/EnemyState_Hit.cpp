#include "EnemyState_Hit.h"
#include"../EnemyState_Idle/EnemyState_Idle.h"

void EnemyState_Hit::StateStart()
{
	EnemyStateBase::StateStart();

	auto anime = m_enemy->GetAnimeModel()->GetAnimation("Hit");
	m_enemy->GetAnimator()->SetAnimation(anime, 0.25f, false);
}

void EnemyState_Hit::StateUpdate()
{
	m_enemy->SetAnimeSpeed(60.0f);

	if (m_enemy->GetAnimator()->IsAnimationEnd())
	{
		// Idleステートに移行
		auto spIdleState = std::make_shared<EnemyState_Idle>();
		m_enemy->ChangeState(spIdleState);
		return;
	}

	if (m_time < 0.2f)
	{
		const float dashSpeed = -1.0f;
		m_enemy->SetIsMoving(m_attackDirection * dashSpeed);
	}
	else
	{
		m_enemy->SetIsMoving(Math::Vector3::Zero);
	}
}

void EnemyState_Hit::StateEnd()
{
}
