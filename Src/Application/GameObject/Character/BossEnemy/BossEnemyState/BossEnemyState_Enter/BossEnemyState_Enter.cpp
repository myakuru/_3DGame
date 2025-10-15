#include "BossEnemyState_Enter.h"
#include"../BossEnemyState_Idle/BossEnemyState_Idle.h"

void BossEnemyState_Enter::StateStart()
{
	auto anime = m_bossEnemy->GetAnimeModel()->GetAnimation("Anim_Enter");
	m_bossEnemy->GetAnimator()->SetAnimation(anime, 0.25f, false);
	BossEnemyStateBase::StateStart();
	// アニメーション速度を変更
	m_bossEnemy->SetAnimeSpeed(60.0f);
}

void BossEnemyState_Enter::StateUpdate()
{
	if (m_bossEnemy->GetAnimator()->IsAnimationEnd())
	{
		auto state = std::make_shared<BossEnemyState_Idle>();
		m_bossEnemy->ChangeState(state);
		return;
	}

	m_bossEnemy->SetIsMoving(Math::Vector3::Zero);
}

void BossEnemyState_Enter::StateEnd()
{
}
