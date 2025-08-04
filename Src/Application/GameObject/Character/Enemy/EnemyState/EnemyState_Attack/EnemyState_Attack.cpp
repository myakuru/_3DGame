#include "EnemyState_Attack.h"
#include"../EnemyState_Attack1/EnemyState_Attack1.h"

void EnemyState_Attack::StateStart()
{
	auto anime = m_enemy->GetAnimeModel()->GetAnimation("Attack0");
	m_enemy->GetAnimator()->AnimationBlend(anime, 10.0f, false);
	m_enemy->AnimeSetFlg() = true;
}

void EnemyState_Attack::StateUpdate()
{
	m_enemy->SetAnimeSpeed(120.0f);

	// 移動量リセット
	m_enemy->SetIsMoving(Math::Vector3::Zero);

	if (m_enemy->GetAnimator()->IsAnimationEnd())
	{
		auto attack1 = std::make_shared<EnemyState_Attack1>();
		m_enemy->ChangeState(attack1);
		return;
	}

}

void EnemyState_Attack::StateEnd()
{
}
