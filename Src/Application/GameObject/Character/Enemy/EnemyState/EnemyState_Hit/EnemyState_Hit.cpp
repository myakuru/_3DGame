#include "EnemyState_Hit.h"
#include"../EnemyState_Idle/EnemyState_Idle.h"

void EnemyState_Hit::StateStart()
{
	auto anime = m_enemy->GetAnimeModel()->GetAnimation("Damage");
	m_enemy->GetAnimator()->AnimationBlend(anime, 10.0f, false);
	m_enemy->AnimeSetFlg() = true;
}

void EnemyState_Hit::StateUpdate()
{
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
}
