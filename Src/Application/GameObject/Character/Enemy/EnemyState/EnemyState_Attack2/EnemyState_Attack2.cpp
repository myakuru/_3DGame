#include "EnemyState_Attack2.h"
#include"../EnemyState_Attack3/EnemState_Attack3.h"

void EnemyState_Attack2::StateStart()
{
	EnemyStateBase::StateStart();

	auto anime = m_enemy->GetAnimeModel()->GetAnimation("Attack2");
	m_enemy->GetAnimator()->SetAnimation(anime, 0.25f, false);
}

void EnemyState_Attack2::StateUpdate()
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
	if (!m_hasHitPlayer && m_time >= 0.0f && m_time <= 0.5f)
	{
		m_enemy->UpdateAttack();
		m_enemy->SetOnceEffect(false);
		m_hasHitPlayer = true;
	}

	if (m_enemy->GetAnimator()->IsAnimationEnd())
	{
		auto attack = std::make_shared<EnemState_Attack3>();
		m_enemy->ChangeState(attack);
		return;
	}

	if (m_time < 0.2f)
	{
		const float dashSpeed = 2.0f;
		m_enemy->SetIsMoving(m_attackDirection * dashSpeed);
	}
	else
	{
		m_enemy->SetIsMoving(Math::Vector3::Zero);
	}

}

void EnemyState_Attack2::StateEnd()
{

}
