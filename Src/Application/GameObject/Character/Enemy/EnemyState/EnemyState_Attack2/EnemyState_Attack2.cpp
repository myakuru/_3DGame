#include "EnemyState_Attack2.h"
#include"../EnemyState_Idle/EnemyState_Idle.h"

void EnemyState_Attack2::StateStart()
{
	auto anime = m_enemy->GetAnimeModel()->GetAnimation("Attack2");
	m_enemy->GetAnimator()->SetAnimation(anime, false);
	m_enemy->AnimeSetFlg() = true;
	m_enemy->m_onceEffect = false;

}

void EnemyState_Attack2::StateUpdate()
{
	//m_enemy->SetAnimeSpeed(60.0f);

	float deltaTime = Application::Instance().GetDeltaTime();

	m_time += deltaTime;

	// 0.5秒間当たり判定有効
	if (m_time <= 1.0 / 2)
	{
		m_enemy->UpdateAttack();
	}

	// 移動量リセット
	m_enemy->SetIsMoving(Math::Vector3::Zero);

	if (m_enemy->GetAnimator()->IsAnimationEnd())
	{
		auto attack = std::make_shared<EnemyState_Idle>();
		m_enemy->ChangeState(attack);
		return;
	}

}

void EnemyState_Attack2::StateEnd()
{

}
