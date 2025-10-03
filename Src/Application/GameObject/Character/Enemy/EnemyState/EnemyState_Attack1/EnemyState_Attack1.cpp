#include "EnemyState_Attack1.h"
#include"../EnemyState_Attack2/EnemyState_Attack2.h"

#include"../../../../../main.h"

void EnemyState_Attack1::StateStart()
{
	EnemyStateBase::StateStart();

	auto anime = m_enemy->GetAnimeModel()->GetAnimation("Attack1");
	m_enemy->GetAnimator()->SetAnimation(anime, 0.25f, false);
	m_enemy->m_onceEffect = false;

	m_time = 0.0f;
}

void EnemyState_Attack1::StateUpdate()
{
	// 攻撃中の移動方向で回転を更新
	if (m_enemy->GetMovement() != Math::Vector3::Zero)
	{
		Math::Vector3 moveDir = m_enemy->GetMovement();
		moveDir.y = 0.0f;
		moveDir.Normalize();
		m_enemy->UpdateQuaternionDirect(moveDir);
	}

	// 0.5秒間当たり判定有効
	if (m_time >= 0.5 && m_time <= 0.7)
	{
		m_enemy->UpdateAttack();
	}

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
