#include "EnemState_Attack3.h"
#include"../EnemyState_Idle/EnemyState_Idle.h"

void EnemState_Attack3::StateStart()
{
	EnemyStateBase::StateStart();

	auto anime = m_enemy->GetAnimeModel()->GetAnimation("Attack3");
	m_enemy->GetAnimator()->SetAnimation(anime, 0.25f, false);
	m_enemy->SetAnimeSpeed(60.0f);
}

void EnemState_Attack3::StateUpdate()
{
	m_enemy->SetAnimeSpeed(60.0f);
	
	// 移動量リセット
	m_enemy->SetIsMoving(Math::Vector3::Zero);
	if (m_enemy->GetAnimator()->IsAnimationEnd())
	{
		auto attack = std::make_shared<EnemyState_Idle>();
		m_enemy->ChangeState(attack);
		return;
	}
}

void EnemState_Attack3::StateEnd()
{
}
