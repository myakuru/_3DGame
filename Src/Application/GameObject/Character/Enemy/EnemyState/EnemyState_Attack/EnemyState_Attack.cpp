#include "EnemyState_Attack.h"
#include"../EnemyState_Attack1/EnemyState_Attack1.h"

void EnemyState_Attack::StateStart()
{
	EnemyStateBase::StateStart();

	auto anime = m_enemy->GetAnimeModel()->GetAnimation("Attack");
	m_enemy->GetAnimator()->SetAnimation(anime, 0.25f, false);
	m_enemy->m_onceEffect = false;
}

void EnemyState_Attack::StateUpdate()
{
	float deltaTime = Application::Instance().GetDeltaTime();

	m_time += deltaTime;

	// 0.5秒間当たり判定有効
	if (m_time >= 0.5 && m_time <= 0.7)
	{
		m_enemy->UpdateAttack();
	}

	if (m_enemy->GetAnimator()->IsAnimationEnd())
	{
		auto attack1 = std::make_shared<EnemyState_Attack1>();
		m_enemy->ChangeState(attack1);
		return;
	}

	// 移動量リセット
	m_enemy->SetIsMoving(Math::Vector3::Zero);

}

void EnemyState_Attack::StateEnd()
{
}
