#include "BossEnemyState_Run.h"
#include"../BossEnemyState_Idle/BossEnemyState_Idle.h"
#include"../BossEnemyState_Attack_L/BossEnemyState_Attack_L.h"
#include"../../../Player/Player.h"

void BossEnemyState_Run::StateStart()
{
	auto anime = m_bossEnemy->GetAnimeModel()->GetAnimation("Walk");
	m_bossEnemy->GetAnimator()->SetAnimation(anime, 0.25f, true);
	BossEnemyStateBase::StateStart();
	// アニメーション速度を変更
	m_bossEnemy->SetAnimeSpeed(60.0f);
}

void BossEnemyState_Run::StateUpdate()
{
	if (auto player = m_bossEnemy->GetPlayerWeakPtr().lock(); player)
	{
		m_playerPos = player->GetPos();
		m_enemyPos = m_bossEnemy->GetPos();
	}

	// 距離計算
	m_distance = (m_playerPos - m_enemyPos).Length();

	if (m_distance >= 10.0f)
	{
		// 追いかける
		Math::Vector3 dir = m_playerPos - m_enemyPos;
		dir.y = 0.0f;
		if (dir != Math::Vector3::Zero) dir.Normalize();

		// LookRotationで正しい向きに
		Math::Quaternion rot = Math::Quaternion::LookRotation(dir, Math::Vector3::Up);
		m_bossEnemy->SetRotation(rot);

		m_bossEnemy->SetIsMoving(dir);
	}
	else
	{
		//Attackステートに移行
		auto attack = std::make_shared<BossEnemyState_Attack_L>();
		m_bossEnemy->ChangeState(attack);
		return;
	}
}

void BossEnemyState_Run::StateEnd()
{
}
