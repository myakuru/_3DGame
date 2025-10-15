#include "BossEnemyState_Idle.h"
#include"../../../Player/Player.h"
#include"../BossEnemyState_Run/BossEnemyState_Run.h"

void BossEnemyState_Idle::StateStart()
{
	auto anime = m_bossEnemy->GetAnimeModel()->GetAnimation("Idle");
	m_bossEnemy->GetAnimator()->SetAnimation(anime, 0.25f, true);
	BossEnemyStateBase::StateStart();
	// アニメーション速度を変更
	m_bossEnemy->SetAnimeSpeed(60.0f);
}

void BossEnemyState_Idle::StateUpdate()
{
	float deltaTime = Application::Instance().GetDeltaTime();

	m_time += deltaTime;

	// 移動量リセット
	m_bossEnemy->SetIsMoving(Math::Vector3::Zero);

	auto player = m_bossEnemy->GetPlayerWeakPtr();
	Math::Vector3 playerPos = player.lock()->GetPos();
	Math::Vector3 enemyPos = m_bossEnemy->GetPos();

	// 距離計算
	m_distance = (playerPos - enemyPos).Length();

	if (m_distance < 20.0f)
	{
		if (m_time <= 3.0f) return;

		// 近ければ追いかける
		auto chaseState = std::make_shared<BossEnemyState_Run>();
		m_bossEnemy->ChangeState(chaseState);
		return;
	}
}

void BossEnemyState_Idle::StateEnd()
{
}
