#include "BossEnemyState_WaterAttack_end.h"
#include"../BossEnemyState_Idle/BossEnemyState_Idle.h"

void BossEnemyState_WaterAttack_end::StateStart()
{
	auto anime = m_bossEnemy->GetAnimeModel()->GetAnimation("Dodge");
	m_bossEnemy->GetAnimator()->SetAnimation(anime, 0.25f, false);
	BossEnemyStateBase::StateStart();
	// アニメーション速度を変更
	m_bossEnemy->SetAnimeSpeed(60.0f);
	// 当たり判定リセット
	m_bossEnemy->ResetAttackCollision();
}

void BossEnemyState_WaterAttack_end::StateUpdate()
{
	// アニメーション再生時間を取得
	m_animeTime = m_bossEnemy->GetAnimator()->GetPlayProgress();

	// アニメーション時間の35％から100％の間、攻撃判定有効
	if (m_animeTime >= 0.35f && m_animeTime <= 1.0f)
	{
		m_bossEnemy->UpdateAttackCollision(10.0f, 1.0f, 1, 0.3f);
	}

	// アニメーションが終了したらIdleへ遷移
	if (m_bossEnemy->GetAnimator()->IsAnimationEnd())
	{
		auto state = std::make_shared<BossEnemyState_Idle>();
		m_bossEnemy->ChangeState(state);
		return;
	}

	m_bossEnemy->SetIsMoving(Math::Vector3::Zero);
}

void BossEnemyState_WaterAttack_end::StateEnd()
{
	m_bossEnemy->SetStateChange(false);
}
