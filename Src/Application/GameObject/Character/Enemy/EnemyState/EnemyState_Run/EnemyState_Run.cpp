#include "EnemyState_Run.h"
#include"../../../Player/Player.h"
#include"../EnemyState_Idle/EnemyState_Idle.h"
#include"../EnemyState_Attack/EnemyState_Attack.h"

void EnemyState_Run::StateStart()
{
	auto anime = m_enemy->GetAnimeModel()->GetAnimation("Run");
	m_enemy->GetAnimator()->AnimationBlend(anime, 10.0f);
	m_enemy->AnimeSetFlg() = true;
}

void EnemyState_Run::StateUpdate()
{
	auto player = m_enemy->GetPlayerWeakPtr();
	Math::Vector3 playerPos = player.lock()->GetPos();
	Math::Vector3 enemyPos = m_enemy->GetPos();

	// 距離計算
	float distance = (playerPos - enemyPos).Length();

	if (distance >= 1.0f && distance < 6.0f)
	{
		// 追いかける
		Math::Vector3 dir = playerPos - enemyPos;
		dir.y = 0.0f;	// 上下に回転してほしくないからこれする。
		dir.Normalize();

		float yaw = std::atan2(-dir.x, -dir.z);

		// Y軸回転クォータニオンを作成											y軸					角度
		Math::Quaternion rotMat = Math::Quaternion::CreateFromAxisAngle(Math::Vector3(0, 1, 0), yaw);

		// 回転行列をセット
		m_enemy->SetRotation(rotMat);

		// 移動方向をセット
		m_enemy->SetIsMoving(dir);
	}
	else if (distance >= 6.0f)
	{
		// Idleステートに移行
		auto spIdleState = std::make_shared<EnemyState_Idle>();
		m_enemy->ChangeState(spIdleState);
		return;
	}
	else
	{
		// Runステートに移行
		auto attack = std::make_shared<EnemyState_Attack>();
		m_enemy->ChangeState(attack);
		return;
	}

}

void EnemyState_Run::StateEnd()
{
	m_enemy->AnimeSetFlg() = false;
}
