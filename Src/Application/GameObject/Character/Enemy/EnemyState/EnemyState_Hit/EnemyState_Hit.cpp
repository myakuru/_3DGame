#include "EnemyState_Hit.h"
#include"../EnemyState_Idle/EnemyState_Idle.h"

void EnemyState_Hit::StateStart()
{
	auto anime = m_enemy->GetAnimeModel()->GetAnimation("Damage");
	m_enemy->GetAnimator()->SetAnimation(anime, false);
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

	m_enemy->SetIsMoving(Math::Vector3::Zero);
}

void EnemyState_Hit::StateEnd()
{
	m_enemy->SetAnimeSpeed(120.0f);
}
