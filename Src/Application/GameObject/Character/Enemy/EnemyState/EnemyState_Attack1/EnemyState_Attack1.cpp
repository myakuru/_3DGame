#include "EnemyState_Attack1.h"
#include"../EnemyState_Attack2/EnemyState_Attack2.h"

#include"../../../../../main.h"

void EnemyState_Attack1::StateStart()
{
	auto anime = m_enemy->GetAnimeModel()->GetAnimation("Attack1");
	m_enemy->GetAnimator()->SetAnimation(anime, false);
	m_enemy->AnimeSetFlg() = true;

	m_enemy->m_onceEffect = false;

	m_time = 0.0f;
}

void EnemyState_Attack1::StateUpdate()
{
	//m_enemy->SetAnimeSpeed(120.0f);

	float deltaTime = Application::Instance().GetDeltaTime();

	m_time += deltaTime;

	// 0.5秒間当たり判定有効
	if (m_time <= 1.0 / 2)
	{
		m_enemy->UpdateAttack();
	}

	if (m_enemy->GetAnimator()->IsAnimationEnd())
	{
		auto attack = std::make_shared<EnemyState_Attack2>();
		m_enemy->ChangeState(attack);
		return;
	}

	// 移動量リセット
	m_enemy->SetIsMoving(Math::Vector3::Zero);
}

void EnemyState_Attack1::StateEnd()
{
}
