#include "EnemyState_Idle.h"

void EnemyState_Idle::StateStart()
{
	auto anime = m_enemy->GetAnimeModel()->GetAnimation("Idle");
	m_enemy->GetAnimator()->AnimationBlend(anime, 10.0f);
	m_enemy->AnimeSetFlg() = true;
}

void EnemyState_Idle::StateUpdate()
{
	// 移動量リセット
	m_enemy->SetIsMoving(Math::Vector3::Zero);
}

void EnemyState_Idle::StateEnd()
{
}
