#include "BossEnemyState_WaterAttack.h"
#include"../BossEnemyState_WaterAttack_end/BossEnemyState_WaterAttack_end.h"

void BossEnemyState_WaterAttack::StateStart()
{
	auto anime = m_bossEnemy->GetAnimeModel()->GetAnimation("Attack_Water");
	m_bossEnemy->GetAnimator()->SetAnimation(anime, 0.25f, false);
	BossEnemyStateBase::StateStart();
	// アニメーション速度を変更
	m_bossEnemy->SetAnimeSpeed(60.0f);
	// 当たり判定リセット
	m_bossEnemy->ResetAttackCollision();
	m_bossEnemy->SetStateChange(true);
}

void BossEnemyState_WaterAttack::StateUpdate()
{
	float deltaTime = Application::Instance().GetDeltaTime();
	m_time += deltaTime;

	if (m_time < 0.3f)
	{
		const float dashSpeed = -5.0f;
		m_bossEnemy->SetIsMoving(m_attackDirection * dashSpeed);
	}
	else
	{
		m_bossEnemy->SetIsMoving(Math::Vector3::Zero);
	}

	// アニメーション終了で次のステートへ
	if (m_bossEnemy->GetAnimator()->IsAnimationEnd())
	{
		auto nextState = std::make_shared<BossEnemyState_WaterAttack_end>();
		m_bossEnemy->ChangeState(nextState);
		return;
	}
}

void BossEnemyState_WaterAttack::StateEnd()
{
}
