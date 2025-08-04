#include "EnemyState_Attack1.h"
#include"../EnemyState_Attack2/EnemyState_Attack2.h"

void EnemyState_Attack1::StateStart()
{
	auto anime = m_enemy->GetAnimeModel()->GetAnimation("Attack1");
	m_enemy->GetAnimator()->AnimationBlend(anime, 10.0f, false);
	m_enemy->AnimeSetFlg() = true;
}

void EnemyState_Attack1::StateUpdate()
{
	m_enemy->SetAnimeSpeed(120.0f);

	// 移動量リセット
	m_enemy->SetIsMoving(Math::Vector3::Zero);

	if (m_enemy->GetAnimator()->IsAnimationEnd())
	{
		auto attack2 = std::make_shared<EnemyState_Attack2>();
		m_enemy->ChangeState(attack2);
		return;
	}
}

void EnemyState_Attack1::StateEnd()
{
	m_enemy->AnimeSetFlg() = false;
}
